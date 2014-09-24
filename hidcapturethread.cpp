//USBHIDDeviceplugin
//Copyright (C) 2014  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include "usbhiddevice.h"
#include "hidcapturethread.h"
#include <QMutex>

HIDCaptureThread::HIDCaptureThread(unsigned short productID,unsigned short vendorID,QObject *parent) : QThread(parent)
{
	m_bActivateJoystickTrigger = false;
	m_bActivateButtonTriggers = false;
	m_cButtonMask = 255;
	m_ButtonDetectCaptureMethod = USBHIDDeviceNameSpace::MaskedValueChanged;
	m_bIsRunning = false;
	m_bAbortRunning = false;
	m_ProductID = productID;
	m_VendorID = vendorID;
	m_bWriteToFile = false;
	m_bWriteHeaderInfo = true;
	m_bWriteFilteredData = true;
	m_qsFileName = "log.txt";
	m_pFile = NULL;
	cOldButtonByteVal = 0;
	cNewButtonByteVal = 0;
	m_bActivateJoystickStabilisation = false;
	m_nJoystickStabilisationThreshold = 0;
	m_bActivateJoystickHistory = false;
	m_nJoystickHistorySize = 0;
}

HIDCaptureThread::~HIDCaptureThread()
{
	stop();
	CloseOutputFile();
	m_JostickHistory.clear();
}

void HIDCaptureThread::stop()
{
	if(this==NULL)
		return;
	if(m_bIsRunning)
	{
		m_bAbortRunning = true;
		int nRetries = 10;//How long are we going to wait for the thread to exit?
		while(m_bIsRunning)
		{
			QThread::msleep(100);
			nRetries--;
			if (nRetries == 0)
				break;
		}
	}
}

void HIDCaptureThread::CloseOutputFile()
{
	FileMutex.lock();
	if(m_pFile)
	{
		m_pFile->close();
		m_pFile = NULL;
	}
	FileMutex.unlock();
}

void HIDCaptureThread::ConfigureHIDTriggers(bool bActivateJoystickTrigger, bool bActivateButtonTriggers, unsigned char cButtonMask, USBHIDDeviceNameSpace::CaptureMethod ButtonDetectCaptureMethod)
{
	m_bActivateJoystickTrigger = bActivateJoystickTrigger;
	m_bActivateButtonTriggers = bActivateButtonTriggers;
	m_cButtonMask = cButtonMask;
	m_ButtonDetectCaptureMethod = ButtonDetectCaptureMethod;
}

void HIDCaptureThread::ConfigureHIDFiltering(bool bActivateJoystickStabilisation, int nJoystickStabilisationThreshold, bool bActivateJoystickHistory, int nJoystickHistorySize)
{
	m_bActivateJoystickStabilisation = bActivateJoystickStabilisation;
	m_nJoystickStabilisationThreshold = nJoystickStabilisationThreshold;
	m_bActivateJoystickHistory = bActivateJoystickHistory;
	m_nJoystickHistorySize = nJoystickHistorySize;
}

void HIDCaptureThread::WriteCapturedDataToFile(bool bWriteToFile, QString qsFileName, bool bWriteHeaderInfo, bool bWriteFilteredData)
{
	m_bWriteToFile = bWriteToFile;
	m_bWriteHeaderInfo = bWriteHeaderInfo;
	m_bWriteFilteredData = bWriteFilteredData;
	if ((qsFileName != "") && (qsFileName != m_qsFileName))
	{
		m_qsFileName = qsFileName;
		CloseOutputFile();
	}
}

bool HIDCaptureThread::OutputCapturedData()
{
	if(!m_bWriteToFile)
		return true;

	QString qsDataPacket = "";
	FileMutex.lock();
	if(m_pFile==NULL)
	{
		m_pFile = new QFile(m_qsFileName);
		if (!m_pFile->open(QIODevice::WriteOnly|QIODevice::Text))//QIODevice::Append |
		{
			m_pFile=NULL;
			qCritical("HIDCaptureThread: Could not write to the file");
		}
		else
		{
			if(m_bWriteHeaderInfo)
			{
				qsDataPacket.append(QString("X\tY\tButton\tTimeStamp\n"));
			}
		}
	}
	FileMutex.unlock();

	if(m_bWriteFilteredData)
	{
		qsDataPacket.append(QString("%02\t").arg(m_HIDFilteredData[0]));
		qsDataPacket.append(QString("%02\t").arg(m_HIDFilteredData[1]));
		qsDataPacket.append(QString("%02\t").arg(m_HIDFilteredData[2]));
		//qsDataPacket.append(QString("%02").arg(m_HIDFilteredData[3]));
	}
	else
	{
		qsDataPacket.append(QString("%02\t").arg((m_HIDData[0])*(int)RESSCALEFACTOR));
		qsDataPacket.append(QString("%02\t").arg((m_HIDData[1])*(int)RESSCALEFACTOR));
		qsDataPacket.append(QString("%02\t").arg(m_HIDData[2]));
		//qsDataPacket.append(QString("%02").arg(m_HIDData[3]));
	}
	qsDataPacket.append(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	QTextStream stream( m_pFile );
	stream << qsDataPacket << "\n";
	return true;
}

void HIDCaptureThread::EmitFilteredData()
{
	m_HIDFilteredData[0] = m_HIDData[0];
	m_HIDFilteredData[1] = m_HIDData[1];
	m_HIDFilteredData[2] = m_HIDData[2];
	m_HIDFilteredData[3] = m_HIDData[3];
	int nRecordCount = 1;
	//Data gets multiplied by RESSCALEFACTOR(to increase the resolution) and optionally filtered by a history(low pass)
	if (m_bActivateJoystickHistory)
	{
		int nCurrentHistorySize = m_JostickHistory.size();
		if (nCurrentHistorySize > 0)
		{
			for (int i=0;i<nCurrentHistorySize;i++)
			{
				m_HIDFilteredData[0] = m_HIDFilteredData[0] + m_JostickHistory.at(i).Xpos;
				m_HIDFilteredData[1] = m_HIDFilteredData[1] + m_JostickHistory.at(i).Ypos;
				nRecordCount++;
			}
		}
	}
	m_HIDFilteredData[0] = (int)(((m_HIDFilteredData[0]*RESSCALEFACTOR)/nRecordCount) + 0.5);//To increase the resolution, allowing filtering and also data consistency
	m_HIDFilteredData[1] = (int)(((m_HIDFilteredData[1]*RESSCALEFACTOR)/nRecordCount) + 0.5);
	emit receiveThreadJoystickChanged(m_HIDFilteredData[0],m_HIDFilteredData[1]);//Send the trigger
}

void HIDCaptureThread::run()
{
	bool bAutoGenerateData = false;//For testing purpose
	bool bButtonValueChanged = false;
	USBHIDDeviceNameSpace::strcJoystickPosition currentJoyPos;
	m_bIsRunning = true;
	emit receiveThreadStarted(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));

	int res;
	unsigned char buf[255];
	hid_device *handle;

	if(!bAutoGenerateData)
	{
		memset(buf,0x00,sizeof(buf));
		struct hid_device_info *devs, *cur_dev;
		devs = hid_enumerate(0x0, 0x0);
		cur_dev = devs;	
		//while (cur_dev) {
		//	printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		//	printf("\n");
		//	printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		//	printf("  Product:      %ls\n", cur_dev->product_string);
		//	printf("  Release:      %hx\n", cur_dev->release_number);
		//	printf("  Interface:    %d\n",  cur_dev->interface_number);
		//	printf("\n");
		//	cur_dev = cur_dev->next;
		//}
		hid_free_enumeration(devs);

		// Open the device using the VID, PID, and optionally the Serial number.
		handle = hid_open(m_VendorID, m_ProductID, NULL);
		if (!handle) {
			//printf("unable to open device\n");
			emit receiveThreadStopped(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
			qCritical("HIDCaptureThread: Could not open HID Device");
			m_bIsRunning = false;
			return;
		}
		// Set the hid_read() function to be non-blocking.
		hid_set_nonblocking(handle, 1);
	}
	else
	{
		m_HIDData[0] = 0;
		m_HIDData[1] = 0;
		m_HIDData[2] = 0;
		m_HIDData[3] = 0;
	}
	while(m_bAbortRunning==false)
	{
		// Try to read from the device. There should be no
		// data here, but execution should not block.
		if(!bAutoGenerateData)
		{
			res = hid_read(handle, buf, sizeof(buf));
		}
		else
		{
			res = 4;//We are going to generate these results
		}
		if (res>1)
		{
			if(!bAutoGenerateData)
			{
				//emit captureThreadTriggered();
				if(buf[0] > 127)
				{
					m_HIDData[0] = (buf[0] - 128);
				}
				else
				{
					m_HIDData[0] = (buf[0] + 128);
				}
				if(buf[1] > 127)
				{
					m_HIDData[1] = (buf[1] - 128);
				}
				else
				{
					m_HIDData[1] = (buf[1] + 128);
				}
				m_HIDData[2] = buf[2];
				m_HIDData[3] = buf[3];
				//Hereafter the center is at position(x,y) (127|128),(127|128) --> (m_HIDData[0],m_HIDData[1])
				//The left point(x) is < 127, right point(x) is > 128 --> m_HIDData[0]
				//The top point(y) is < 127, bottom point(y) is > 128 --> m_HIDData[1]
				//The Buttons are at m_HIDData[2]; LeftButton = 1, RightButton = 2
				//TriggerInput is also at m_HIDData[2], emulates Button;(BNC; inside = TTL, outside = GND) only rising edge(send only a single packet as response)!

			}
			else//Generate some dummy data for testing
			{
				m_HIDData[0] = m_HIDData[0]+1;
				m_HIDData[1] = m_HIDData[1]+1;
				//m_HIDData[2] = m_HIDData[2]+2;
				//m_HIDData[3] = m_HIDData[3]+2;
			}		

			if(m_bActivateButtonTriggers)
			{
				unsigned char cSignalVal;
				cNewButtonByteVal = (m_HIDData[2] & m_cButtonMask);
				switch (m_ButtonDetectCaptureMethod)
				{
				case USBHIDDeviceNameSpace::MaskedValueChanged :
					if (cOldButtonByteVal != cNewButtonByteVal)//any change within masked measurement
					{
						emit receiveThreadButtonTriggered(cNewButtonByteVal,cOldButtonByteVal ^ cNewButtonByteVal);//Signal(Difference, Press and Release)
						bButtonValueChanged = true;						
					}
					cOldButtonByteVal = cNewButtonByteVal;
					break;	
				case USBHIDDeviceNameSpace::MaskedValueChangedHigh :
					cSignalVal = (~cOldButtonByteVal) & cNewButtonByteVal;
					if(cSignalVal)//any change from low to high
					{
						emit receiveThreadButtonTriggered(cNewButtonByteVal,cSignalVal);//Signal(Positive Changes, Press)
						bButtonValueChanged = true;
					}
					cOldButtonByteVal = cNewButtonByteVal;
					break;
				case USBHIDDeviceNameSpace::MaskedValueChangedLow :
					cSignalVal = cOldButtonByteVal & (~cNewButtonByteVal);
					if(cSignalVal)//any change from high to low
					{
						emit receiveThreadButtonTriggered(cNewButtonByteVal,cSignalVal);//Signal(Positive Changes, Release)
						bButtonValueChanged = true;
					}
					cOldButtonByteVal = cNewButtonByteVal;
					break;	
				}
			}

			//Maintain a history
			currentJoyPos.Xpos = m_HIDData[0];
			currentJoyPos.Ypos = m_HIDData[1];
			while(m_JostickHistory.size() > m_nJoystickHistorySize)
			{
				m_JostickHistory.pop_front();//Maximum size reached
			}
			if (m_bActivateJoystickTrigger)//Is the trigger activated?
			{
				if(m_JostickHistory.size()>0)//Do we already have a history?
				{
					if(!((m_JostickHistory.last().Xpos == currentJoyPos.Xpos) && (m_JostickHistory.last().Ypos == currentJoyPos.Ypos)))//Only when value changed!
					{
						if((m_bActivateJoystickStabilisation)&&(m_nJoystickStabilisationThreshold>0))
						{
							if( ((m_JostickHistory.last().Xpos - m_nJoystickStabilisationThreshold) > currentJoyPos.Xpos) || \
								((m_JostickHistory.last().Xpos + m_nJoystickStabilisationThreshold) < currentJoyPos.Xpos) || \
							    ((m_JostickHistory.last().Ypos - m_nJoystickStabilisationThreshold) > currentJoyPos.Ypos) || \
								((m_JostickHistory.last().Ypos + m_nJoystickStabilisationThreshold) < currentJoyPos.Ypos) )
								//Only when value changed enough!!	
							{
								EmitFilteredData();//Send the trigger
								if (!OutputCapturedData()) m_bAbortRunning = true;
							}							
						}
						else
						{
							EmitFilteredData();//Send the trigger
							if (!OutputCapturedData()) m_bAbortRunning = true;
						}
					}					
				}
				else
				{
					EmitFilteredData();//Send the trigger, first record!
					if (!OutputCapturedData()) m_bAbortRunning = true;
				}
			}
			//else if ((m_bActivateButtonTriggers) && (bButtonValueChanged))
			//{
			//	
			//}
			m_JostickHistory.append(currentJoyPos);//Now add the new data record
		}
		//QThread::msleep(10);
	}
	if(!bAutoGenerateData)
		hid_close(handle);
	CloseOutputFile();
	m_bAbortRunning = false;
	m_bIsRunning = false;
	emit receiveThreadStopped(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	return;
}

