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


//This file inherits the Interface description, please don't change

#ifndef USBHIDDevicePLUGIN_H
#define USBHIDDevicePLUGIN_H

#include <QObject>
#include <QtWidgets>
#include <QString>
#include <Qlibrary>

#include "plugininterface.h"
#include "usbhiddevice_dialog.h"
#include "usbhiddevice.h"
#include "defines.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class USBHIDDevicePlugin : public QObject, DeviceInterface
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "BrainStim.Plugins.Devices" "usbhiddevice.json")
    Q_INTERFACES(DeviceInterface)

public:
	USBHIDDevicePlugin(QObject *parent = 0);
	~USBHIDDevicePlugin();

	int ConfigureScriptEngine(QScriptEngine &engine);
	QString GetMinimalMainProgramVersion() {return PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION;};

private:
	short nExampleProperty;
	USBHIDDevice *USBHIDDeviceObject; 
	USBHIDDevice_Dialog *USBHIDDeviceDiagObject;

public slots:
	bool HasGUI() { return true; };
    bool ShowGUI();
	bool IsCompatible() {return PluginInterface::IsCompatible();};
	QObject *GetScriptMetaObject(int nIndex) {if(nIndex == 0) return (QObject *)USBHIDDeviceObject->metaObject(); else return NULL;};
};

#endif//USBHIDDevicePLUGIN_H
