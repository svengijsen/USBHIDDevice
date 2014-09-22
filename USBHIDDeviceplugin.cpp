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


#include "USBHIDDeviceplugin.h"
#include "USBHIDDevice.h"
#include "defines.h"

Q_DECLARE_METATYPE(USBHIDDevice*)
Q_DECLARE_METATYPE(USBHIDDevice)

USBHIDDevicePlugin::USBHIDDevicePlugin(QObject *parent)
{
	USBHIDDeviceObject = new USBHIDDevice(0x181b, 0x4002, parent);
	USBHIDDeviceDiagObject = new USBHIDDevice_Dialog();	
	strPluginInformation = PLUGIN_INFORMATION;
}

USBHIDDevicePlugin::~USBHIDDevicePlugin()
{
	delete USBHIDDeviceDiagObject;
	delete USBHIDDeviceObject;
}

int USBHIDDevicePlugin::ConfigureScriptEngine(QScriptEngine &engine)
{
	QScriptValue USBHIDDeviceProto = engine.newQObject(USBHIDDeviceObject);
	engine.setDefaultPrototype(qMetaTypeId<USBHIDDevice*>(), USBHIDDeviceProto);
	QScriptValue USBHIDDeviceCtor = engine.newFunction(USBHIDDevice::ctor__extensionname, USBHIDDeviceProto);
	engine.globalObject().setProperty(PLUGIN_SCRIPTOBJECT_NAME, USBHIDDeviceCtor);
	int nMetaType = qRegisterMetaType<USBHIDDevice>(PLUGIN_SCRIPTOBJECT_CLASS);
	return nMetaType;
}

bool USBHIDDevicePlugin::ShowGUI()
{
	int returnVal;
	USBHIDDeviceDiagObject->setWindowTitle(strPluginInformation);
	returnVal = USBHIDDeviceDiagObject->exec();

	switch (returnVal) {
	case QMessageBox::Save:
	   // Save was clicked
	   break;
	case QMessageBox::Discard:
		// Don't Save was clicked
	    break;
	case QMessageBox::Cancel:
	    // Cancel was clicked
	    break;
	default:
	    // should never be reached
	    break;
	}		
	return true;
}

//Q_PLUGIN_METADATA(USBHIDDeviceplugin, USBHIDDevicePlugin);
