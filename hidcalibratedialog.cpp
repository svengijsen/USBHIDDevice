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

#include "hidcalibratedialog.h"
#include <QFileDialog>

HIDCalibrateDialog::HIDCalibrateDialog(USBHIDDevice *pUSBHIDDEV, QWidget *parent) : QDialog(parent)
{
	m_bSuccesfullyInitialized = false;
	m_bCalibrationActive = false;
	m_USBHIDDev = pUSBHIDDEV;
	ui.setupUi(this);
	setWindowTitle(tr("HID Calibration"));
	ResetCurrentCalibration();
	layout = new QHBoxLayout(ui.groupBox);
	renderArea = new RenderArea;
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonClicked()));
	connect(ui.startButton, SIGNAL(clicked()), this, SLOT(StartButtonClicked()));
	connect(ui.stopButton, SIGNAL(clicked()), this, SLOT(StopButtonClicked()));
	connect(m_USBHIDDev, SIGNAL(CalibrateThreadStarted(QString)), this, SLOT(CalibrationStarted(QString)));
	connect(m_USBHIDDev, SIGNAL(CalibrateThreadStopped(QString)), this, SLOT(CalibrationStopped(QString)));
	connect(m_USBHIDDev, SIGNAL(CaptureThreadButtonTriggered(unsigned char, unsigned char)), this, SLOT(HIDButtonPressed(unsigned char,unsigned char)));	
	ui.label->setText("Ready to start the calibration.\nPress the 'Start' button or the Joysticks Left button to start the calibration.\nPress the Joysticks Right button to exit the calibration.");
	m_bSuccesfullyInitialized = Initialize();
}

bool HIDCalibrateDialog::Initialize()
{
	bool bRetVal = false;
	m_USBHIDDev->ConfigureHIDTriggers(true,true,255,1);//Low->High
	m_USBHIDDev->ConfigureHIDFiltering(false,0,false,0);
	bRetVal = m_USBHIDDev->StartCaptureThread(true);
	if (!bRetVal)
	{
		ui.label->setText("Could not initialize the HID Device!!");
	}
	else
	{
		m_USBHIDDev->SetCalibrationData(m_CalibrationConfiguration);
	}
	return bRetVal;
}

void HIDCalibrateDialog::ResetCurrentCalibration()
{
	m_CalibrationConfiguration.Xmin = 255 * RESSCALEFACTOR;
	m_CalibrationConfiguration.Ymin = 255 * RESSCALEFACTOR;
	m_CalibrationConfiguration.Xmax = 0;
	m_CalibrationConfiguration.Ymax = 0;
	m_CalibrationConfiguration.XAbsOffset = 0;
	m_CalibrationConfiguration.YAbsOffset = 0;
	m_CalibrationConfiguration.XRelScaleFactor = 1;
	m_CalibrationConfiguration.YRelScaleFactor = 1;
}

void HIDCalibrateDialog::HIDButtonPressed(unsigned char cButtonValue,unsigned char cChangedByteValue)
{
	Q_UNUSED(cButtonValue);
	if (cChangedByteValue & 1)//Left HID Button pressed;
	{
		StartCalibration();
	}
	else if (cChangedByteValue & 2)//Right HID Button pressed;
	{
		if (!m_bCalibrationActive)
		{
			this->reject();
		}
		else
		{
			StopCalibration();
		}
	}
}

HIDCalibrateDialog::~HIDCalibrateDialog()
{
	delete layout;

	disconnect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));
	disconnect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonClicked()));
	disconnect(ui.startButton, SIGNAL(clicked()), this, SLOT(StartButtonClicked()));
	disconnect(ui.stopButton, SIGNAL(clicked()), this, SLOT(StopButtonClicked()));
	disconnect(m_USBHIDDev, SIGNAL(CalibrateThreadStarted(QString)), this, SLOT(CalibrationStarted(QString)));
	disconnect(m_USBHIDDev, SIGNAL(CalibrateThreadStopped(QString)), this, SLOT(CalibrationStopped(QString)));
	disconnect(m_USBHIDDev, SIGNAL(CaptureThreadButtonTriggered(unsigned char,unsigned char)), this, SLOT(HIDButtonPressed(unsigned char,unsigned char)));

	StopCalibration();
	if(m_bSuccesfullyInitialized)
	{
		m_USBHIDDev->StopCaptureThread(true);
	}
	delete renderArea;
}

void HIDCalibrateDialog::StopButtonClicked()
{
	StopCalibration();
}

void HIDCalibrateDialog::StartButtonClicked()
{
	StartCalibration();
}

void HIDCalibrateDialog::CalibrationStarted(QString sMessage)
{
	//ui.label->setText("Calibration Thread started.\nPress the 'Stop' button or the Joysticks Right button to stop the calibration.");
}

void HIDCalibrateDialog::CalibrationStopped(QString sMessage)
{
	//ui.label->setText("Calibration Thread stopped.\nPress the 'Start' button or the Joysticks Left button to start the calibration again.\nPress the 'OK' button to Apply the calibrated setting or 'Cancel' to exit without\n applying the new calibration settings.");
}

void HIDCalibrateDialog::UpdateCalibration(int nXPos, int nYPos)
{
	bool bCalChanged = false;
	if (nXPos < m_CalibrationConfiguration.Xmin)
	{
		m_CalibrationConfiguration.Xmin = nXPos;
		bCalChanged = true;
	}
	if (nYPos < m_CalibrationConfiguration.Ymin)
	{
		m_CalibrationConfiguration.Ymin = nYPos;
		bCalChanged = true;
	}
	if (nXPos > m_CalibrationConfiguration.Xmax)
	{
		m_CalibrationConfiguration.Xmax = nXPos;
		bCalChanged = true;
	}
	if (nYPos > m_CalibrationConfiguration.Ymax)
	{
		m_CalibrationConfiguration.Ymax = nYPos;
		bCalChanged = true;
	}
	if((m_CalibrationConfiguration.Xmin < m_CalibrationConfiguration.Xmax) && (m_CalibrationConfiguration.Ymin < m_CalibrationConfiguration.Ymax))
	{
		if(m_bCalibrationActive)
		{
			if(bCalChanged)
			{
				m_CalibrationConfiguration.XRelScaleFactor = (float)(RESSCALEFACTOR/(m_CalibrationConfiguration.Xmax - m_CalibrationConfiguration.Xmin)); //0 ... RESSCALEFACTOR
				m_CalibrationConfiguration.YRelScaleFactor = (float)(RESSCALEFACTOR/(m_CalibrationConfiguration.Ymax - m_CalibrationConfiguration.Ymin)); //0 ... RESSCALEFACTOR
				m_CalibrationConfiguration.XAbsOffset = ((m_CalibrationConfiguration.Xmax - m_CalibrationConfiguration.Xmin)/2)+m_CalibrationConfiguration.Xmin;
				m_CalibrationConfiguration.YAbsOffset = ((m_CalibrationConfiguration.Ymax - m_CalibrationConfiguration.Ymin)/2)+m_CalibrationConfiguration.Ymin;
			}			
			renderArea->UpdatePosition((nXPos-m_CalibrationConfiguration.Xmin)*m_CalibrationConfiguration.XRelScaleFactor,(nYPos-m_CalibrationConfiguration.Ymin)*m_CalibrationConfiguration.YRelScaleFactor);
		}		
	}
}

void HIDCalibrateDialog::StartCalibration()
{
	if ((!m_bCalibrationActive) && (m_bSuccesfullyInitialized))
	{
		layout->addWidget(renderArea);
		connect(m_USBHIDDev, SIGNAL(CaptureThreadJoystickChanged(int, int)), this, SLOT(UpdateCalibration(int, int)));
		m_bCalibrationActive = true;
		ui.label->setText("Calibration started.\nPress the 'Stop' button or the Joysticks Right button to stop the calibration.");
	}
}

void HIDCalibrateDialog::StopCalibration()
{
	if (m_bCalibrationActive)
	{
		layout->removeWidget(renderArea);
		disconnect(m_USBHIDDev, SIGNAL(CaptureThreadJoystickChanged(unsigned char, unsigned char)), this, SLOT(UpdateCalibration(unsigned char, unsigned char)));
		m_bCalibrationActive = false;
		ui.label->setText("Calibration stopped.\nPress the 'Start' button or the Joysticks Left button to start the calibration again.\nPress the 'OK' button to Apply the calibrated setting or 'Cancel' to exit without\n applying the new calibration settings.");
	}	
}

void HIDCalibrateDialog::OkButtonClicked()
{
	if(m_bSuccesfullyInitialized)
	{
		ApplyCalibrationData();
		StopCalibration();
	}
}

void HIDCalibrateDialog::CancelButtonClicked()
{
	if(m_bSuccesfullyInitialized)
	{
		StopCalibration();
	}
}

void HIDCalibrateDialog::ApplyCalibrationData()
{
	if(m_bSuccesfullyInitialized)
	{
		m_USBHIDDev->SetCalibrationData(m_CalibrationConfiguration);
	}
}