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


//This file defines the script binding interface, all below function are scriptable except for the destructor

#ifndef USBHIDDevice_H
#define USBHIDDevice_H
#include <QObject>
#include <QString>
#include <QtScript>
#include <QScriptable>
#include <QDesktopWidget>
#include <QMouseEvent>
#include "mainappinfo.h"
#include "hidapi.h"
#include "hidcalibratedialog.h"
#include "hidcapturethread.h"
//#include "Global.h"

class HIDCalibrateDialog;
//!  The USBHIDDevice class. 
/*!
  The USBHIDDevice can be used to communicate with a USB HID Device.
*/
class USBHIDDevice : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "USBHIDDevice");//Can't use defines here!, moc doesn't handle defines, not needed here
										 
signals:
	//! The CaptureThreadStarted Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread is started.
		Parameter DateTimeStamp holds the date/time of when the generate thread started.
	*/
	void CaptureThreadStarted(QString DateTimeStamp);
	//! The CaptureThreadStopped Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread is stopped.
		Parameter DateTimeStamp holds the date/time of when the generate thread stopped.
	*/
	void CaptureThreadStopped(QString DateTimeStamp);
	//! The CalibrateThreadStarted Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread calibration is started.
		Parameter DateTimeStamp holds the date/time of when the generate thread calibration started.
	*/
	void CalibrateThreadStarted(QString DateTimeStamp);
	//! The CalibrateThreadStopped Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread calibration is stopped.
		Parameter DateTimeStamp holds the date/time of when the generate thread calibration stopped.
	*/
	void CalibrateThreadStopped(QString DateTimeStamp);
	//! The CaptureThreadJoystickChanged Signal.
	/*!
		This signal is generated when the capture thread detects a change in the X,Y position of the Joystick.
		The Xpos and Ypos hold the last value of the Joystick when the change was detected.
	*/
	void CaptureThreadJoystickChanged(int Xpos, int Ypos);
	//! The CaptureThreadButtonTriggered Signal.
	/*!
		This signal is generated when the capture thread detects a change in the Buttons.
		NewValue holds the new value of the Buttons after the change was detected,
		ChangedBits holds a mask of the Button-bits that changed value.
	*/
	void CaptureThreadButtonTriggered(unsigned char NewValue, unsigned char ChangedBits);

public:
	USBHIDDevice(unsigned short vendor_id = 0x181b, unsigned short product_id = 0x4002, QObject *parent = 0);
	USBHIDDevice(const USBHIDDevice& other ){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~USBHIDDevice();

	static QScriptValue ctor__extensionname(QScriptContext* context, QScriptEngine* engine);
	void SetCalibrationData(const USBHIDDeviceNameSpace::strcCalibrationData &CalData);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	bool Calibrate();
	bool StartCaptureThread(bool bIsCalibrationThread = false);
	void StopCaptureThread(bool bWasCalibrationThread = false);
	void SetVendorID(unsigned short vendor_id);
	void SetProductID(unsigned short product_id);

	void WriteCapturedDataToFile(bool bWriteToFile, QString qsFileName, bool bWriteHeaderInfo, bool bWriteFilteredData);
	void ConfigureHIDTriggers(bool bActivateJoystickTrigger, bool bActivateButtonTriggers, unsigned char cButtonMask, short ButtonDetectCaptureMethod);
	void ConfigureHIDFiltering(bool bActivateJoystickStabilisation, int nJoystickStabilisationThreshold, bool bActivateJoystickHistory, int nJoystickHistorySize);
	void EmulateHIDMouse(bool bEnable, bool bFullScreenMode, bool bEnableLeftMouse = false, bool bEnableRightMouse = false);
	void ResetCalibrationData();
	
private slots:
	void UpdateHIDMouseEmuPosition(int XPos, int YPos);
	void UpdateHIDMouseEmuButtons(unsigned char ButtonByteValue,unsigned char ChangedByteValue);
	
private:
	QScriptEngine* currentScriptEngine;
	HIDCalibrateDialog *HIDCalDiag;
	HIDCaptureThread *HIDCapThread;
	bool m_bActivateJoystickTrigger;
	bool m_bActivateButtonTriggers;
	unsigned char m_cButtonMask;
	short m_sButtonDetectCaptureMethod;
	bool m_bActivateJoystickStabilisation;
	int m_nJoystickStabilisationThreshold;
	bool m_bActivateJoystickHistory;
	bool m_bActivateMousePosEmu;
	bool m_bActivateMouseLButtonEmu;
	bool m_bActivateMouseRButtonEmu;
	QPoint mouseRelPos;
	int m_nJoystickHistorySize;
	bool m_bMouseFullScreenMode;
	QRect rScreenResolution;
	QMutex MouseEmuMutex;

	unsigned short m_VendorId;
	unsigned short m_ProductId;
	bool m_bWriteToFile;
	bool m_bWriteHeaderInfo;
	bool m_bWriteCalibratedData;
	QString m_qsFileName;
	USBHIDDeviceNameSpace::strcCalibrationData m_CalData;  
	int m_nXCalcHIDPos;
	int m_nYCalcHIDPos;
	float m_fScreenWidthStep;
	float m_fScreenHeightStep;
};

#endif // USBHIDDevice_H
