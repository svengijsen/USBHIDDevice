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


#include "usbhiddevice_dialog.h"

USBHIDDevice_Dialog::USBHIDDevice_Dialog(QWidget *parent) : QDialog(parent)
{
	//Gets constructed only once during the load of the plugin
	ui.setupUi(this);
	layout = new QHBoxLayout(ui.groupBox_Calibration);
	renderArea = new RenderArea;
	bDoReject = false;
	bDoAccept = false;
	bCalibrationActive = false;
	ResetCurrentCalibration();
	ui.pushButton_Calibrate->setPalette(Qt::red);
	ui.pushButton_Calibrate->setEnabled(true);
	ChangeCurrentState(USBHIDDeviceNameSpace::NotInitialized);
	UI_USBHIDDeviceObject = NULL;
	connect(ui.checkBox_EnableFilter, SIGNAL(clicked()), this, SLOT(checkBox_EnableFilter_Pressed()));
	connect(ui.checkBox_ToLog, SIGNAL(clicked()), this, SLOT(checkBox_ToLog_Pressed()));
	connect(ui.checkBox_MouseEmulation, SIGNAL(clicked()), this, SLOT(checkBox_MouseEmulation_Pressed()));
	connect(ui.checkBox_FullScreen, SIGNAL(clicked()), this, SLOT(checkBox_FullScreen_Pressed()));
	connect(ui.checkBox_LeftMouse, SIGNAL(clicked()), this, SLOT(checkBox_LeftMouse_Pressed()));
	connect(ui.checkBox_RightMouse, SIGNAL(clicked()), this, SLOT(checkBox_RightMouse_Pressed()));
	connect(ui.checkBox_EnableStab, SIGNAL(clicked()), this, SLOT(checkBox_EnableStab_Pressed()));
	connect(ui.checkBox_ShowGraph, SIGNAL(clicked()), this, SLOT(checkBox_ShowGraph_Pressed()));
	connect(ui.checkBox_ShowRef, SIGNAL(clicked()), this, SLOT(checkBox_ShowRef_Pressed()));
	connect(ui.pushButton_Capture, SIGNAL(clicked()), this, SLOT(pushButton_Capture_Pressed()));
	connect(ui.pushButton_Stop, SIGNAL(clicked()), this, SLOT(pushButton_Stop_Pressed()));
	connect(ui.pushButton_ResetCal, SIGNAL(clicked()), this, SLOT(pushButton_ResetCal_Pressed()));
	connect(ui.pushButton_Calibrate, SIGNAL(clicked()), this, SLOT(pushButton_Calibrate_Pressed()));
	connect(ui.pushButton_Browse, SIGNAL(clicked()), this, SLOT(pushButton_Browse_Pressed()));
}

USBHIDDevice_Dialog::~USBHIDDevice_Dialog()
{
	delete layout;
	DestroyHIDDevice();
	disconnect(ui.checkBox_EnableFilter, SIGNAL(clicked()), this, SLOT(checkBox_EnableFilter_Pressed()));
	disconnect(ui.checkBox_ToLog, SIGNAL(clicked()), this, SLOT(checkBox_ToLog_Pressed()));
	disconnect(ui.checkBox_MouseEmulation, SIGNAL(clicked()), this, SLOT(checkBox_MouseEmulation_Pressed()));
	disconnect(ui.checkBox_FullScreen, SIGNAL(clicked()), this, SLOT(checkBox_FullScreen_Pressed()));
	disconnect(ui.checkBox_LeftMouse, SIGNAL(clicked()), this, SLOT(checkBox_LeftMouse_Pressed()));
	disconnect(ui.checkBox_RightMouse, SIGNAL(clicked()), this, SLOT(checkBox_RightMouse_Pressed()));
	disconnect(ui.checkBox_EnableStab, SIGNAL(clicked()), this, SLOT(checkBox_EnableStab_Pressed()));
	disconnect(ui.checkBox_ShowGraph, SIGNAL(clicked()), this, SLOT(checkBox_ShowGraph_Pressed()));
	disconnect(ui.checkBox_ShowRef, SIGNAL(clicked()), this, SLOT(checkBox_ShowRef_Pressed()));
	disconnect(ui.pushButton_Capture, SIGNAL(clicked()), this, SLOT(pushButton_Capture_Pressed()));
	disconnect(ui.pushButton_Stop, SIGNAL(clicked()), this, SLOT(pushButton_Stop_Pressed()));
	disconnect(ui.pushButton_ResetCal, SIGNAL(clicked()), this, SLOT(pushButton_ResetCal_Pressed()));
	disconnect(ui.pushButton_Calibrate, SIGNAL(clicked()), this, SLOT(pushButton_Calibrate_Pressed()));
	disconnect(ui.pushButton_Browse, SIGNAL(clicked()), this, SLOT(pushButton_Browse_Pressed()));
	delete renderArea;
}

void USBHIDDevice_Dialog::DestroyHIDDevice()
{
	if (UI_USBHIDDeviceObject)
	{
		DisconnectSignalSlots();
		delete UI_USBHIDDeviceObject;
		UI_USBHIDDeviceObject = NULL;
	}
}

void USBHIDDevice_Dialog::on_okButton_clicked()
{
	bDoAccept = true;
	StopCapture();
	if (m_CurrentState == USBHIDDeviceNameSpace::ReadyToCapture || m_CurrentState == USBHIDDeviceNameSpace::NotInitialized)
	{
		bDoAccept = false;
		accept();
	} 
}

void USBHIDDevice_Dialog::on_cancelButton_clicked()
{
	bDoReject = true;
	StopCapture();
	if (m_CurrentState == USBHIDDeviceNameSpace::ReadyToCapture || m_CurrentState == USBHIDDeviceNameSpace::NotInitialized)
	{
		bDoReject = false;
		reject();
	}
}

void USBHIDDevice_Dialog::closeEvent(QCloseEvent * e)
{
	Q_UNUSED(e);
	on_cancelButton_clicked();
}

void USBHIDDevice_Dialog::keyPressEvent(QKeyEvent *event)
{
	//Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
	//bool isSHIFT = keyMod.testFlag(Qt::ShiftModifier);
	//bool isCTRL = keyMod.testFlag(Qt::ControlModifier);

	//if (isSHIFT && isCTRL)
	//{
	//	// to-do smth
	//}
	switch (event->key())
	{
	case Qt::Key_Escape:
		if (ui.checkBox_MouseEmulation->isChecked())
		{
			ui.checkBox_MouseEmulation->setChecked(false);
			checkBox_MouseEmulation_Pressed();
		}
		break;
	}
	event->ignore();
}

void USBHIDDevice_Dialog::ChangeCurrentState(USBHIDDeviceNameSpace::HIDDevDialogState currState)
{
	switch (currState)
	{
	case USBHIDDeviceNameSpace::NotInitialized:
		ui.groupBox_Comm->setEnabled(true);
		//ui.groupBox_Cap->setEnabled(true);
		ui.groupBox_Filter->setEnabled(true);
		//ui.groupBox_Graph->setEnabled(true);
		//ui.groupBox_Cal->setEnabled(true);
		ui.groupBox_Logging->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Capture->setEnabled(true);
		ui.label_State->setText("Pending...");
		break;
	case USBHIDDeviceNameSpace::ReadyToCapture:
		ui.groupBox_Comm->setEnabled(true);
		//ui.groupBox_Cap->setEnabled(true);
		ui.groupBox_Filter->setEnabled(true);
		//ui.groupBox_Graph->setEnabled(true);
		//ui.groupBox_Cal->setEnabled(true);
		ui.groupBox_Logging->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Capture->setEnabled(true);
		ui.label_State->setText("Ready to capture...");
		break;
	case USBHIDDeviceNameSpace::CaptureStarting:
		ui.groupBox_Comm->setEnabled(false);
		//ui.groupBox_Cap->setEnabled(true);
		ui.groupBox_Filter->setEnabled(false);
		//ui.groupBox_Graph->setEnabled(true);
		//ui.groupBox_Cal->setEnabled(true);
		ui.groupBox_Logging->setEnabled(false);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Capture->setEnabled(false);
		ui.label_State->setText("Capture Thread is starting...");
		break;
	case USBHIDDeviceNameSpace::CaptureStarted:
		ui.groupBox_Comm->setEnabled(false);
		//ui.groupBox_Cap->setEnabled(true);
		ui.groupBox_Filter->setEnabled(false);
		//ui.groupBox_Graph->setEnabled(true);
		//ui.groupBox_Cal->setEnabled(true);
		ui.groupBox_Logging->setEnabled(false);
		ui.pushButton_Stop->setEnabled(true);
		ui.pushButton_Capture->setEnabled(false);
		ui.label_State->setText("Capture Thread is running...");
		break;
	case USBHIDDeviceNameSpace::CaptureStopping:
		ui.groupBox_Comm->setEnabled(false);
		//ui.groupBox_Cap->setEnabled(true);
		ui.groupBox_Filter->setEnabled(false);
		//ui.groupBox_Graph->setEnabled(true);
		//ui.groupBox_Cal->setEnabled(true);
		ui.groupBox_Logging->setEnabled(false);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Capture->setEnabled(false);
		ui.label_State->setText("Capture Thread is stopping...");
		break;
	}
	m_CurrentState = currState;//NotInitialized;
}

//////////////////////////////////////////////// UI //////////////////////////////////////////////////////////////

void USBHIDDevice_Dialog::pushButton_Capture_Pressed()
{
	StartCapture();
}

void USBHIDDevice_Dialog::pushButton_Stop_Pressed()
{
	StopCapture();
}

void USBHIDDevice_Dialog::pushButton_ResetCal_Pressed()
{
	ResetCurrentCalibration();
}

void USBHIDDevice_Dialog::pushButton_Browse_Pressed()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), "", tr("Log Files (*.txt *.log)"));
	if (!(fileName == ""))
	{
		ui.lineEdit_LogPath->setText(fileName);
	}
}

void USBHIDDevice_Dialog::pushButton_Calibrate_Pressed()
{
	bCalibrationActive = !bCalibrationActive;
	if(bCalibrationActive)
	{
		ui.pushButton_Calibrate->setText("Disable Calibration");
		ui.pushButton_Calibrate->setPalette(Qt::green);
	}
	else
	{
		ui.pushButton_Calibrate->setText("Enable Calibration");
		ui.pushButton_Calibrate->setPalette(Qt::red);
	}
}

void USBHIDDevice_Dialog::checkBox_EnableFilter_Pressed()
{
	ui.lineEdit_HistorySize->setEnabled(ui.checkBox_EnableFilter->isChecked());
}

void USBHIDDevice_Dialog::checkBox_ToLog_Pressed()
{
	ui.checkBox_WriteFil->setEnabled(ui.checkBox_ToLog->isChecked());
	ui.checkBox_WriteHeader->setEnabled(ui.checkBox_ToLog->isChecked());
	ui.pushButton_Browse->setEnabled(ui.checkBox_ToLog->isChecked());
	ui.lineEdit_LogPath->setEnabled(ui.checkBox_ToLog->isChecked());
}

void USBHIDDevice_Dialog::checkBox_MouseEmulation_Pressed()
{
	//ui.checkBox_FullScreen->setEnabled(ui.checkBox_MouseEmulation->isChecked());
	//ui.checkBox_RightMouse->setEnabled(ui.checkBox_MouseEmulation->isChecked());
	//ui.checkBox_LeftMouse->setEnabled(ui.checkBox_MouseEmulation->isChecked());
	if(UI_USBHIDDeviceObject)
		UI_USBHIDDeviceObject->EmulateHIDMouse(ui.checkBox_MouseEmulation->isChecked(),ui.checkBox_FullScreen->isChecked(),ui.checkBox_LeftMouse->isChecked(),ui.checkBox_RightMouse->isChecked());
}

void USBHIDDevice_Dialog::checkBox_FullScreen_Pressed()
{
	if(UI_USBHIDDeviceObject)
		UI_USBHIDDeviceObject->EmulateHIDMouse(ui.checkBox_MouseEmulation->isChecked(),ui.checkBox_FullScreen->isChecked(),ui.checkBox_LeftMouse->isChecked(),ui.checkBox_RightMouse->isChecked());
}

void USBHIDDevice_Dialog::checkBox_LeftMouse_Pressed()
{
	if(UI_USBHIDDeviceObject)
		UI_USBHIDDeviceObject->EmulateHIDMouse(ui.checkBox_MouseEmulation->isChecked(),ui.checkBox_FullScreen->isChecked(),ui.checkBox_LeftMouse->isChecked(),ui.checkBox_RightMouse->isChecked());
}
void USBHIDDevice_Dialog::checkBox_RightMouse_Pressed()
{
	if(UI_USBHIDDeviceObject)
		UI_USBHIDDeviceObject->EmulateHIDMouse(ui.checkBox_MouseEmulation->isChecked(),ui.checkBox_FullScreen->isChecked(),ui.checkBox_LeftMouse->isChecked(),ui.checkBox_RightMouse->isChecked());
}

void USBHIDDevice_Dialog::checkBox_EnableStab_Pressed()
{
	ui.lineEdit_ThresholdValue->setEnabled(ui.checkBox_EnableStab->isChecked());
}

void USBHIDDevice_Dialog::checkBox_ShowGraph_Pressed()
{

}

void USBHIDDevice_Dialog::checkBox_ShowRef_Pressed()
{

}

//////////////////////////////////////////////// HID SLOTS /////////////////////////////////////////////////////////

void USBHIDDevice_Dialog::CaptureHasStarted(QString sMessage)
{
	layout->addWidget(renderArea);
	ChangeCurrentState(USBHIDDeviceNameSpace::CaptureStarted);
}

void USBHIDDevice_Dialog::CaptureHasStopped(QString sMessage)
{
	layout->removeWidget(renderArea);
	ChangeCurrentState(USBHIDDeviceNameSpace::ReadyToCapture);
	DestroyHIDDevice();
	ChangeCurrentState(USBHIDDeviceNameSpace::NotInitialized);
	if(bDoReject)
	{
		bDoReject = false;
		reject();
	}
	if(bDoAccept)
	{
		bDoAccept = false;
		accept();
	}	
}

void USBHIDDevice_Dialog::HIDButtonPressed(unsigned char cButtonValue,unsigned char cChangedByteValue)
{
	Q_UNUSED(cChangedByteValue);
	QString tmpString;
	ui.lineEdit_Buttons->setText(tmpString.setNum(cButtonValue,10));
}

void USBHIDDevice_Dialog::HIDJoystickChanged(int nXpos, int nYpos)
{
	QString tmpString;
	ui.lineEdit_Xpos->setText(tmpString.setNum(nXpos,10));
	ui.lineEdit_Ypos->setText(tmpString.setNum(nYpos,10));
	if (bCalibrationActive)
	{
		UpdateCalibration(nXpos,nYpos);
	}
	if (ui.checkBox_ShowGraph->isChecked())
	{
		renderArea->UpdatePosition((nXpos-m_CalibrationConfiguration.Xmin)*m_CalibrationConfiguration.XRelScaleFactor,(nYpos-m_CalibrationConfiguration.Ymin)*m_CalibrationConfiguration.YRelScaleFactor,ui.checkBox_ShowRef->isChecked());
	}
}

//////////////////////////////////////////////// GENERAL //////////////////////////////////////////////////////////

void USBHIDDevice_Dialog::UpdateCalibration(int nXPos, int nYPos)
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
		if(bCalChanged)
		{
			m_CalibrationConfiguration.XRelScaleFactor = (float)(RESSCALEFACTOR/(m_CalibrationConfiguration.Xmax - m_CalibrationConfiguration.Xmin)); //0 ... RESSCALEFACTOR
			m_CalibrationConfiguration.YRelScaleFactor = (float)(RESSCALEFACTOR/(m_CalibrationConfiguration.Ymax - m_CalibrationConfiguration.Ymin)); //0 ... RESSCALEFACTOR
			m_CalibrationConfiguration.XAbsOffset = ((m_CalibrationConfiguration.Xmax - m_CalibrationConfiguration.Xmin)/2)+m_CalibrationConfiguration.Xmin;
			m_CalibrationConfiguration.YAbsOffset = ((m_CalibrationConfiguration.Ymax - m_CalibrationConfiguration.Ymin)/2)+m_CalibrationConfiguration.Ymin;
			UpdateUICalibration();
			UI_USBHIDDeviceObject->SetCalibrationData(m_CalibrationConfiguration);
		}			
	}
}

void USBHIDDevice_Dialog::ResetCurrentCalibration()
{
	m_CalibrationConfiguration.Xmin = 255 * RESSCALEFACTOR;
	m_CalibrationConfiguration.Ymin = 255 * RESSCALEFACTOR;
	m_CalibrationConfiguration.Xmax = 0;
	m_CalibrationConfiguration.Ymax = 0;
	m_CalibrationConfiguration.XAbsOffset = 0;
	m_CalibrationConfiguration.YAbsOffset = 0;
	m_CalibrationConfiguration.XRelScaleFactor = 1;
	m_CalibrationConfiguration.YRelScaleFactor = 1;
	UpdateUICalibration();
}

void USBHIDDevice_Dialog::UpdateUICalibration()
{
	QString tmpString;
	ui.lineEdit_Xmin->setText(tmpString.setNum(m_CalibrationConfiguration.Xmin,10));
	ui.lineEdit_Ymin->setText(tmpString.setNum(m_CalibrationConfiguration.Ymin,10));
	ui.lineEdit_Xmax->setText(tmpString.setNum(m_CalibrationConfiguration.Xmax,10));
	ui.lineEdit_Ymax->setText(tmpString.setNum(m_CalibrationConfiguration.Ymax,10));
	ui.lineEdit_Xoffset->setText(tmpString.setNum(m_CalibrationConfiguration.XAbsOffset,10));
	ui.lineEdit_Yoffset->setText(tmpString.setNum(m_CalibrationConfiguration.YAbsOffset,10));
	ui.lineEdit_Xscale->setText(tmpString.setNum(m_CalibrationConfiguration.XRelScaleFactor,'f',6));
	ui.lineEdit_Yscale->setText(tmpString.setNum(m_CalibrationConfiguration.YRelScaleFactor,'f',6));
}

void USBHIDDevice_Dialog::Initialize()
{
	if (m_CurrentState == USBHIDDeviceNameSpace::NotInitialized)
	{
		bDoAccept = false;
		bDoReject = false;
		UI_USBHIDDeviceObject = new USBHIDDevice(ui.lineEdit_VenID->text().toUShort(), ui.lineEdit_ProdID->text().toUShort(),this);
		ConnectSignalSlots();
		UI_USBHIDDeviceObject->SetCalibrationData(m_CalibrationConfiguration);
		UI_USBHIDDeviceObject->ConfigureHIDFiltering(ui.checkBox_EnableStab->isChecked(),ui.lineEdit_ThresholdValue->text().toInt(),ui.checkBox_EnableFilter->isChecked(),ui.lineEdit_HistorySize->text().toInt());
        UI_USBHIDDeviceObject->ConfigureHIDTriggers(true,true,255,USBHIDDeviceNameSpace::MaskedValueChanged);
		UI_USBHIDDeviceObject->WriteCapturedDataToFile(ui.checkBox_ToLog->isChecked(),ui.lineEdit_LogPath->text(),ui.checkBox_WriteHeader->isChecked(),ui.checkBox_WriteFil->isChecked());
		UI_USBHIDDeviceObject->EmulateHIDMouse(ui.checkBox_MouseEmulation->isChecked(),ui.checkBox_FullScreen->isChecked(),ui.checkBox_LeftMouse->isChecked(),ui.checkBox_RightMouse->isChecked());
		ChangeCurrentState(USBHIDDeviceNameSpace::ReadyToCapture);
	}
}

void USBHIDDevice_Dialog::ConnectSignalSlots()
{
	connect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadStarted(QString)), this, SLOT(CaptureHasStarted(QString)));
	connect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadStopped(QString)), this, SLOT(CaptureHasStopped(QString)));
	connect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadButtonTriggered(unsigned char,unsigned char)), this, SLOT(HIDButtonPressed(unsigned char,unsigned char)));
	connect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadJoystickChanged(int, int)), this, SLOT(HIDJoystickChanged(int, int)));
}

void USBHIDDevice_Dialog::DisconnectSignalSlots()
{
	disconnect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadStarted(QString)), this, SLOT(CaptureHasStarted(QString)));
	disconnect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadStopped(QString)), this, SLOT(CaptureHasStopped(QString)));
	disconnect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadButtonTriggered(unsigned char,unsigned char)), this, SLOT(HIDButtonPressed(unsigned char,unsigned char)));
	disconnect(UI_USBHIDDeviceObject, SIGNAL(CaptureThreadJoystickChanged(int, int)), this, SLOT(HIDJoystickChanged(int, int)));
}

void USBHIDDevice_Dialog::StartCapture()
{
	Initialize();
	if (m_CurrentState == USBHIDDeviceNameSpace::ReadyToCapture)
	{
		if(UI_USBHIDDeviceObject->StartCaptureThread())
		{
			ChangeCurrentState(USBHIDDeviceNameSpace::CaptureStarting);			
		}
		else
		{
			DisconnectSignalSlots();
			delete UI_USBHIDDeviceObject;
			UI_USBHIDDeviceObject = NULL;
			ChangeCurrentState(USBHIDDeviceNameSpace::NotInitialized);
		}
	}
}

void USBHIDDevice_Dialog::StopCapture()
{
	if (m_CurrentState == USBHIDDeviceNameSpace::CaptureStarted)
	{
		UI_USBHIDDeviceObject->StopCaptureThread();
		ChangeCurrentState(USBHIDDeviceNameSpace::CaptureStopping);
	}
}
