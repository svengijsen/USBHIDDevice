//USBHIDDeviceplugin
//Copyright (C) 2015  Sven Gijsen
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


#ifndef HIDCAPTURETHREAD_H
#define HIDCAPTURETHREAD_H

#define PACKET_SIZE 4

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QList>
#include "global.h"

class HIDCaptureThread : public QThread
{
	Q_OBJECT

protected:
	void run();

public:

	HIDCaptureThread(unsigned short productID,unsigned short vendorID,QObject *parent);
	~HIDCaptureThread();

public slots:
	void WriteCapturedDataToFile(bool bWriteToFile = true, QString qsFileName = "", bool bWriteHeaderInfo = true, bool bWriteFilteredData = true);
	void ConfigureHIDTriggers(bool bActivateJoystickTrigger, bool bActivateButtonTriggers, unsigned char cButtonMask, USBHIDDeviceNameSpace::CaptureMethod ButtonDetectCaptureMethod);
	void ConfigureHIDFiltering(bool bActivateJoystickStabilisation, int nJoystickStabilisationThreshold, bool bActivateJoystickHistory, int nJoystickHistorySize);
	void stop();

private:
	bool OutputCapturedData();
	void CloseOutputFile();
	void EmitFilteredData();

	bool m_bIsRunning;
	bool m_bAbortRunning;
	bool m_bWriteToFile;
	bool m_bWriteHeaderInfo;
	bool m_bWriteFilteredData;
	bool m_bActivateJoystickTrigger;
	bool m_bActivateButtonTriggers;
	QString m_qsFileName;
	unsigned short m_ProductID;
	unsigned short m_VendorID;
	unsigned char m_HIDData[PACKET_SIZE];
	int m_HIDFilteredData[PACKET_SIZE];
	unsigned char m_cButtonMask;
	USBHIDDeviceNameSpace::CaptureMethod m_ButtonDetectCaptureMethod;
	QFile *m_pFile;
	QMutex FileMutex;
	QList<USBHIDDeviceNameSpace::strcJoystickPosition> m_JostickHistory;//x,y
	unsigned char cOldButtonByteVal;
	unsigned char cNewButtonByteVal;
	bool m_bActivateJoystickStabilisation;
	int m_nJoystickStabilisationThreshold;
	bool m_bActivateJoystickHistory;
	int m_nJoystickHistorySize;

signals:
	//void receiveThreadTriggered(QString sData);
	void receiveThreadJoystickChanged(int XValue, int YValue);
	void receiveThreadButtonTriggered(unsigned char ButtonByteValue,unsigned char ChangedByteValue);
	void receiveThreadStarted(QString timeTextStamp);
	void receiveThreadStopped(QString timeTextStamp);
};

#endif // HIDCAPTURETHREAD_H
