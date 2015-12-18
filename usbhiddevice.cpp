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


#include "usbhiddevice.h"
#include <windows.h>
#include <QTextStream>

QScriptValue USBHIDDevice::ctor_USBHIDDevice(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	//this function gets called first whenever a new object is constructed trough the script

	//	if (context->isCalledAsConstructor()) {
	//		// initialize the new object
	//		//context->thisObject().setProperty("bar", ...);
	//		// ...
	//		// return a non-object value to indicate that the
	//		// thisObject() should be the result of the "new Foo()" expression
	//		//return engine->undefinedValue();

	return engine->newQObject(new USBHIDDevice(0x181b, 0x4002), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

/*! \brief The USBHIDDevice constructor.
*
*   The vendor_id should be set to the Vendor ID of the USB HID Device,
*   the product_id should be set to the Product ID of the USB HID Device.
*   You do not need to specify the parent object, 
*	the BrainStim script engine automatically retrieves the parent role
*/
USBHIDDevice::USBHIDDevice(unsigned short vendor_id, unsigned short product_id, QObject *parent) : QObject(parent)
{
	currentScriptEngine = NULL;
	m_bActivateJoystickTrigger = false;
	m_bActivateButtonTriggers = false;
	m_bActivateMousePosEmu = false;
	m_bMouseFullScreenMode = false;
	m_bActivateMouseLButtonEmu = false;
	m_bActivateMouseRButtonEmu = false;
	m_cButtonMask = 255;
    m_sButtonDetectCaptureMethod = USBHIDDeviceNameSpace::MaskedValueChanged;
	m_VendorId = vendor_id;
	m_ProductId = product_id;
	m_bWriteToFile = false;
	m_bWriteHeaderInfo = true;
	m_bWriteCalibratedData = true;
	m_qsFileName = "";
	m_bActivateJoystickStabilisation = false;
	m_nJoystickStabilisationThreshold = 0;
	m_bActivateJoystickHistory = false;
	m_nJoystickHistorySize = 0;
	HIDCalDiag = NULL;
	HIDCapThread = NULL;
	//mouseEvent = NULL;
	ResetCalibrationData();
}

/*! \brief The USBHIDDevice destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
USBHIDDevice::~USBHIDDevice()
{
	StopCaptureThread();
	if(HIDCalDiag)
	{
		delete HIDCalDiag;
		HIDCalDiag = NULL;
	}
}

bool USBHIDDevice::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

void USBHIDDevice::SetVendorID(unsigned short vendor_id)
{
	/*! Sets the Vendor ID for the USB HID Device. */
	m_VendorId = vendor_id;
}

void USBHIDDevice::SetProductID(unsigned short product_id)
{
	/*! Sets the Product ID for the USB HID Device. */
	m_ProductId = product_id;
}

bool USBHIDDevice::Initialize(unsigned short vendor_id, unsigned short product_id)
{
	hid_device *handle = hid_open(vendor_id, product_id, NULL);
	if (handle)
	{
		hid_close(handle);
		SetVendorID(vendor_id);
		SetProductID(product_id);
		return true;
	}
	return false;
}

void USBHIDDevice::ResetCalibrationData()
{
	/*! Resets the internal calibration data for the current USB HID Device. */
	m_CalData.Xmin = 255 * RESSCALEFACTOR;
	m_CalData.Xmax = 0;
	m_CalData.Ymin = 255 * RESSCALEFACTOR;
	m_CalData.Ymax = 0;
	m_CalData.XAbsOffset = 0; 
	m_CalData.YAbsOffset = 0;
	m_CalData.XRelScaleFactor = 0.0f;
	m_CalData.YRelScaleFactor = 0.0f;
}

void USBHIDDevice::SetCalibrationData(const USBHIDDeviceNameSpace::strcCalibrationData &CalData)
{
	m_CalData = CalData;
}

//void USBHIDDevice::setExampleProperty( short sExampleProperty )
//{
//	m_ExampleProperty = sExampleProperty;
//	//emit ExampleSignalTriggered(m_ExampleProperty);//Signal that the value has been changed
//}
//
//short USBHIDDevice::getExampleProperty() const
//{
//	return m_ExampleProperty;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool USBHIDDevice::Calibrate()
{
	/*! 
	* Starts a Calibration wizard for an easy calibration for the current USB HID Device. 
	* Returns true if the calibration was successful and accepted by the user.
	*/
	int nResult;
	bool bRetVal = false;

	if (HIDCalDiag == NULL)
		HIDCalDiag = new HIDCalibrateDialog(this);
	nResult = HIDCalDiag->exec();   //->show();
	switch (nResult)
	{
	case QDialog::Accepted://OK Button Pressed
		nResult = 9;
		bRetVal = true;
		break;
	case QDialog::Rejected://Cancel or Close pressed
		nResult = 10;
		break;
	}	
	delete HIDCalDiag;
	HIDCalDiag = NULL;
	return bRetVal;
}

void USBHIDDevice::ConfigureHIDTriggers(bool bActivateJoystickTrigger, bool bActivateButtonTriggers, unsigned char cButtonMask, short sButtonDetectCaptureMethod)
{
	m_bActivateJoystickTrigger = bActivateJoystickTrigger;
	m_bActivateButtonTriggers = bActivateButtonTriggers;
	m_cButtonMask = cButtonMask;
	m_sButtonDetectCaptureMethod = sButtonDetectCaptureMethod;
}

void USBHIDDevice::ConfigureHIDFiltering(bool bActivateJoystickStabilisation, int nJoystickStabilisationThreshold, bool bActivateJoystickHistory, int nJoystickHistorySize)
{
	m_bActivateJoystickStabilisation = bActivateJoystickStabilisation;
	m_nJoystickStabilisationThreshold = nJoystickStabilisationThreshold;
	m_bActivateJoystickHistory = bActivateJoystickHistory;
	m_nJoystickHistorySize = nJoystickHistorySize;
}

void USBHIDDevice::EmulateHIDMouse(bool bEnable, bool bFullScreenMode, bool bEnableLeftMouse, bool bEnableRightMouse)
{
	//Get The screen resolution
	QDesktopWidget* desktopWidget = QApplication::desktop();
	//get client rect.
	rScreenResolution = desktopWidget->screenGeometry();//availableGeometry();
	//get application rect.
	//QRect applicationRect = desktopWidget->screenGeometry();

	if(!m_bActivateMousePosEmu)
	{
		if(bEnable)//When setting from false to true
		{
			connect(this, SIGNAL(CaptureThreadJoystickChanged(int, int)), this, SLOT(UpdateHIDMouseEmuPosition(int, int))); 
			if (bEnableLeftMouse || bEnableRightMouse)
				connect(this, SIGNAL(CaptureThreadButtonTriggered(unsigned char, unsigned char)), this, SLOT(UpdateHIDMouseEmuButtons(unsigned char, unsigned char)));					
			else
				disconnect(this, SIGNAL(CaptureThreadButtonTriggered(unsigned char, unsigned char)), this, SLOT(UpdateHIDMouseEmuButtons(unsigned char, unsigned char)));	
		}
	}
	else if(m_bActivateMousePosEmu)
	{
		if(!bEnable)//When setting from true to false
		{
			disconnect(this, SIGNAL(CaptureThreadJoystickChanged(int, int)), this, SLOT(UpdateHIDMouseEmuPosition(int, int)));
			disconnect(this, SIGNAL(CaptureThreadButtonTriggered(unsigned char, unsigned char)), this, SLOT(UpdateHIDMouseEmuButtons(unsigned char, unsigned char)));	
		}
		
	}
	m_bActivateMousePosEmu = bEnable;
	m_bMouseFullScreenMode = bFullScreenMode;
	m_bActivateMouseLButtonEmu = bEnableLeftMouse;
	m_bActivateMouseRButtonEmu = bEnableRightMouse;
}

void USBHIDDevice::UpdateHIDMouseEmuPosition(int XPos, int YPos)
{
	MouseEmuMutex.lock();
	m_fScreenWidthStep = (rScreenResolution.width()/(RESSCALEFACTOR));
	m_fScreenHeightStep = (rScreenResolution.height()/(RESSCALEFACTOR));

	if(m_bActivateMousePosEmu)
	{
		m_nXCalcHIDPos = m_fScreenWidthStep * ((XPos-m_CalData.Xmin)*m_CalData.XRelScaleFactor);
		m_nYCalcHIDPos = m_fScreenHeightStep * ((YPos-m_CalData.Ymin)*m_CalData.YRelScaleFactor);
		if (m_bMouseFullScreenMode)
		{
			m_nXCalcHIDPos = (m_nXCalcHIDPos * SQRRTTWO) - (0.5*(SQRRTTWO-1)*rScreenResolution.width());
			m_nYCalcHIDPos = (m_nYCalcHIDPos * SQRRTTWO) - (0.5*(SQRRTTWO-1)*rScreenResolution.height());
			if (m_nXCalcHIDPos >= rScreenResolution.width())
			{
				m_nXCalcHIDPos = rScreenResolution.width()-1;
			}
			else if (m_nXCalcHIDPos < 0)
			{
				m_nXCalcHIDPos = 0;
			}
			if (m_nYCalcHIDPos >= rScreenResolution.height())
			{
				m_nYCalcHIDPos = rScreenResolution.height()-1;
			}
			else if (m_nYCalcHIDPos < 0)
			{
				m_nYCalcHIDPos = 0;
			}
		}
		QCursor::setPos(m_nXCalcHIDPos,m_nYCalcHIDPos);
	}
	MouseEmuMutex.unlock();
}

void USBHIDDevice::UpdateHIDMouseEmuButtons(unsigned char ButtonByteValue,unsigned char ChangedByteValue)
{
	MouseEmuMutex.lock();
	//if(mouseEvent)
	//{
	//	delete mouseEvent;
	//	mouseEvent = NULL;
	//}
	QMouseEvent* mouseEvent;
	QWidget *widgetUnderCursor;
	if(m_bActivateMouseLButtonEmu)
	{
		if ((ChangedByteValue & 0x01) == 0x01)//Did the Left Mouse Button Change?
		{
			widgetUnderCursor = qApp->widgetAt(QCursor::pos());
			mouseRelPos = widgetUnderCursor->mapFromGlobal(QCursor::pos());

			if((ButtonByteValue & 0x01) == 0x01)//Did it Press?
			{
                mouseEvent = new QMouseEvent( QEvent::MouseButtonPress, mouseRelPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
			}
			else//Was it a Release?
			{
                mouseEvent = new QMouseEvent( QEvent::MouseButtonRelease, mouseRelPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
			}
			QApplication::postEvent( widgetUnderCursor, mouseEvent );	
			delete mouseEvent;
		}
	}
	if(m_bActivateMouseRButtonEmu)
	{
		if ((ChangedByteValue & 0x02) == 0x02)//Did the Right Mouse Button Change?
		{
			widgetUnderCursor = qApp->widgetAt(QCursor::pos());
			mouseRelPos = widgetUnderCursor->mapFromGlobal(QCursor::pos());

			if((ButtonByteValue & 0x02) == 0x02)//Did it Press?
			{
                mouseEvent = new QMouseEvent( QEvent::MouseButtonPress, mouseRelPos, Qt::RightButton, Qt::RightButton, Qt::NoModifier);
			}
			else//Was it a Release?
			{
                mouseEvent = new QMouseEvent( QEvent::MouseButtonRelease, mouseRelPos, Qt::RightButton, Qt::RightButton, Qt::NoModifier);
			}
			QApplication::postEvent( widgetUnderCursor, mouseEvent );	
			delete mouseEvent;
		}
	}
	widgetUnderCursor = NULL;
	MouseEmuMutex.unlock();
}

void USBHIDDevice::WriteCapturedDataToFile(bool bWriteToFile, QString qsFileName, bool bWriteHeaderInfo, bool bWriteCalibratedData)
{
	m_bWriteToFile = bWriteToFile;
	m_bWriteHeaderInfo = bWriteHeaderInfo;
	m_bWriteCalibratedData = bWriteCalibratedData;
	if (qsFileName!="")
		m_qsFileName = qsFileName;
}

bool USBHIDDevice::StartCaptureThread(bool bIsCalibrationThread)
{
	USBHIDDeviceNameSpace::CaptureMethod CapDecMethod;
	if (m_sButtonDetectCaptureMethod == 0) 
    {CapDecMethod = USBHIDDeviceNameSpace::MaskedValueChanged;}
	else if (m_sButtonDetectCaptureMethod == 1) 
    {CapDecMethod = USBHIDDeviceNameSpace::MaskedValueChangedHigh;}
	else 
    {CapDecMethod = USBHIDDeviceNameSpace::MaskedValueChangedLow;}
	if (HIDCapThread)
	{
		delete HIDCapThread;
	}
	HIDCapThread = new HIDCaptureThread(m_ProductId,m_VendorId,this);
	HIDCapThread->WriteCapturedDataToFile(m_bWriteToFile,m_qsFileName,m_bWriteHeaderInfo,m_bWriteCalibratedData);
	HIDCapThread->ConfigureHIDTriggers(m_bActivateJoystickTrigger,m_bActivateButtonTriggers,m_cButtonMask,CapDecMethod);
	HIDCapThread->ConfigureHIDFiltering(m_bActivateJoystickStabilisation,m_nJoystickStabilisationThreshold,m_bActivateJoystickHistory,m_nJoystickHistorySize);

	connect(HIDCapThread, SIGNAL(receiveThreadJoystickChanged(int, int)), this, SIGNAL(CaptureThreadJoystickChanged(int, int)));
	if(bIsCalibrationThread)
	{
		connect(HIDCapThread, SIGNAL(receiveThreadStarted(QString)), this, SIGNAL(CalibrateThreadStarted(QString)));
		connect(HIDCapThread, SIGNAL(receiveThreadStopped(QString)), this, SIGNAL(CalibrateThreadStopped(QString)));
	}
	else
	{
		connect(HIDCapThread, SIGNAL(receiveThreadStarted(QString)), this, SIGNAL(CaptureThreadStarted(QString)));
		connect(HIDCapThread, SIGNAL(receiveThreadStopped(QString)), this, SIGNAL(CaptureThreadStopped(QString)));
	}
	connect(HIDCapThread, SIGNAL(receiveThreadButtonTriggered(unsigned char,unsigned char)), this, SIGNAL(CaptureThreadButtonTriggered(unsigned char,unsigned char)));

	HIDCapThread->start();
	return true;
}

void USBHIDDevice::StopCaptureThread(bool bWasCalibrationThread)
{
	/*! Stops the Capture Thread(if started), see #StartCaptureThread. */
	if(HIDCapThread)
	{
		HIDCapThread->stop();
		disconnect(HIDCapThread, SIGNAL(receiveThreadJoystickChanged(int, int)), this, SIGNAL(CaptureThreadJoystickChanged(int, int)));
		if(bWasCalibrationThread)
		{
			disconnect(HIDCapThread, SIGNAL(receiveThreadStarted(QString)), this, SIGNAL(CalibrateThreadStarted(QString)));
			disconnect(HIDCapThread, SIGNAL(receiveThreadStopped(QString)), this, SIGNAL(CalibrateThreadStopped(QString)));
		}
		else
		{
			disconnect(HIDCapThread, SIGNAL(receiveThreadStarted(QString)), this, SIGNAL(CaptureThreadStarted(QString)));
			disconnect(HIDCapThread, SIGNAL(receiveThreadStopped(QString)), this, SIGNAL(CaptureThreadStopped(QString)));
		}
		disconnect(HIDCapThread, SIGNAL(receiveThreadButtonTriggered(unsigned char,unsigned char)), this, SIGNAL(CaptureThreadButtonTriggered(unsigned char,unsigned char)));
		delete HIDCapThread;
		HIDCapThread = NULL;
	}
}
