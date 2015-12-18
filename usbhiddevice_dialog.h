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


//This file implements the UI functionality

#ifndef USBHIDDEVICE_DIALOG_H
#define USBHIDDEVICE_DIALOG_H

#include <QDialog>
#include "ui_USBHIDDevice_dialog.h"
#include <QtWidgets>
#include <Windows.h>
#include <QDialog>
#include <QThread>
#include "usbhiddevice.h"


class USBHIDDevice_Dialog : public QDialog, private Ui::USBHIDDevice_DialogClass
{
	Q_OBJECT

public:
	USBHIDDevice_Dialog(QWidget *parent = 0);
	~USBHIDDevice_Dialog();

private:
	USBHIDDevice *UI_USBHIDDeviceObject;
	USBHIDDeviceNameSpace::HIDDevDialogState m_CurrentState;
	bool bCalibrationActive;
	bool bDoReject;
	bool bDoAccept;
	QHBoxLayout *layout;
	RenderArea *renderArea;
	USBHIDDeviceNameSpace::strcCalibrationData m_CalibrationConfiguration;
	Ui::USBHIDDevice_DialogClass ui;
	void ConnectSignalSlots();
	void DisconnectSignalSlots();
	void Initialize();
	void ResetCurrentCalibration();
	void DestroyHIDDevice();
	void UpdateUICalibration();
	
private slots:
	void on_cancelButton_clicked();
	void on_okButton_clicked();
	void closeEvent(QCloseEvent * e);
	void keyPressEvent(QKeyEvent *event);
	void checkBox_EnableFilter_Pressed();
	void checkBox_ToLog_Pressed();
	void checkBox_MouseEmulation_Pressed();
	void checkBox_FullScreen_Pressed();
	void checkBox_LeftMouse_Pressed();
	void checkBox_RightMouse_Pressed();
	void checkBox_EnableStab_Pressed();
	void checkBox_ShowGraph_Pressed();
	void checkBox_ShowRef_Pressed();
	void pushButton_Capture_Pressed();
	void pushButton_Stop_Pressed();
	void pushButton_ResetCal_Pressed();
	void pushButton_Calibrate_Pressed();
	void pushButton_Browse_Pressed();

	void ChangeCurrentState(USBHIDDeviceNameSpace::HIDDevDialogState currState);
	void UpdateCalibration(int nXPos, int nYPos);
	void StartCapture();
	void StopCapture();

	void CaptureHasStarted(QString sMessage);
	void CaptureHasStopped(QString sMessage);
	void HIDButtonPressed(unsigned char cButtonValue,unsigned char cChangedByteValue);
	void HIDJoystickChanged(int nXpos, int nYpos);
};

#endif // USBHIDDEVICE_DIALOG_H
