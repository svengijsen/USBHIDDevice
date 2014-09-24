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


#ifndef HIDCALIBRATEDIALOG_H
#define HIDCALIBRATEDIALOG_H

#include <QDialog>
#include "ui_hidcalibratedialog.h"
#include "renderarea.h"
#include "usbhiddevice.h"
#include "global.h"

class USBHIDDevice;
class HIDCalibrateDialog : public QDialog
{
	Q_OBJECT

public:
	HIDCalibrateDialog(USBHIDDevice *pUSBHIDDEV, QWidget *parent = NULL);
	~HIDCalibrateDialog();

private:
	void ApplyCalibrationData();
	void StartCalibration();
	void StopCalibration();
	void ResetCurrentCalibration();
	bool Initialize();
	
	Ui::HIDCalibrateDialog ui;
	QHBoxLayout *layout;
	RenderArea *renderArea;
	USBHIDDevice *m_USBHIDDev;
	bool m_bCalibrationActive;
	bool m_bSuccesfullyInitialized;
	USBHIDDeviceNameSpace::strcCalibrationData m_CalibrationConfiguration;

public slots:
	void UpdateCalibration(int nXPos, int nYPos);
	void CalibrationStarted(QString sMessage);
	void CalibrationStopped(QString sMessage);
	void HIDButtonPressed(unsigned char cButtonValue,unsigned char cChangedByteValue);

private slots:
	void OkButtonClicked();
	void StartButtonClicked();
	void StopButtonClicked();
	void CancelButtonClicked();
};

#endif // HIDCALIBRATEDIALOG_H
