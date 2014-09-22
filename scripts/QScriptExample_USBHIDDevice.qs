//Construct a new Plugin object
ExampleDevicePluginObj = new USBHIDDevice();
//Configure the Plugin Object
ExampleDevicePluginObj.SetVendorID(0x181b);
ExampleDevicePluginObj.SetProductID(0x4002);


//Construct a QTimer
var timer = new QTimer(this);


//Declare some functions
function mySignalFunction()
{
	Log("mySignalFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("mySignalFunction argument: " + arguments[i]); 	
}

function myTriggeredFunction()
{
	Log("myTriggeredFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("myTriggeredFunction argument: " + arguments[i]); 	
}

function myButtonTriggerFunction()
{
	Log("myButtonTriggerFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("myButtonTriggerFunction argument: " + arguments[i]); 	
	nTimerCounter = 1;
}

function CalibrateStopped()
{
	Log("CalibrateStopped: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("CalibrateStopped argument: " + arguments[i]); 	
}

function CaptureStopped()
{
	Log("CaptureStopped: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("CaptureStopped argument: " + arguments[i]); 	
	nTimerCounter = 0;
}

function PauseMills(millis)
{
	var date = new Date();
	var curDate = null;
     
	do { curDate = new Date(); }
	while(curDate-date < millis)
}

var nTimerCounter = 5;
function timerFunction() //The first function demonstrates how a QTimer constructed from within the script can be connected to a custom defined script function
{ 
	Log("timerFunction1() is to be called " + nTimerCounter + "times" ) 
	if(nTimerCounter == 1)
	{
		ExampleDevicePluginObj.StopCaptureThread();
	}
	else if (nTimerCounter == 0)
	{	
		cleanup();
		return;
	}
	nTimerCounter--;
}

function cleanup() //First we define a function for the necessary cleanup before exiting the script
{
	timer.stop(); //Stop the timer
	timer.timeout.disconnect(this, this.timerFunction); //Disconnect again
	timerFunction = null; //cleanup the function
	ExampleDevicePluginObj.CaptureThreadStarted.disconnect(this, this.mySignalFunction); 
	ExampleDevicePluginObj.CaptureThreadStopped.disconnect(this, this.CaptureStopped); 
	//ExampleDevicePluginObj.CaptureThreadJoystickChanged.disconnect(this, this.myTriggeredFunction); 
	ExampleDevicePluginObj.CaptureThreadButtonTriggered.disconnect(this, this.myButtonTriggerFunction);
	CaptureStopped = null;
	CalibrateStopped = null;
	mySignalFunction = null; //Cleanup the function
	myButtonTriggerFunction = null;
	PauseMills = null; //Cleanup the function
	cleanup = null; //Cleanup the function
	//ExampleDevicePluginObj = null;
	Log("CleanUp Finished...");
}

//Example usage of the Example property
//ExampleDevicePluginObj.setExampleProperty(99);
//Log(ExampleDevicePluginObj.ExampleProperty);
//ExampleDevicePluginObj.doMeasurement();
//ExampleDevicePluginObj.ExampleProperty = 88;
//Log(ExampleDevicePluginObj.ExampleProperty);



ExampleDevicePluginObj.Calibrate();
Log("Calibration done...");
//ExampleDevicePluginObj.Calibrate();
//Log("Calibration done...");


//Connect the Signal/Slots
ExampleDevicePluginObj.CaptureThreadStarted.connect(this, this.mySignalFunction); 
ExampleDevicePluginObj.CaptureThreadStopped.connect(this, this.CaptureStopped); 
ExampleDevicePluginObj.CalibrateThreadStopped.connect(this, this.CalibrateStopped); 
//ExampleDevicePluginObj.CaptureThreadJoystickChanged.connect(this, this.myTriggeredFunction); 
ExampleDevicePluginObj.CaptureThreadButtonTriggered.connect(this, this.myButtonTriggerFunction);//unsigned char

ExampleDevicePluginObj.WriteCapturedDataToFile(true,"d:\\temp\\output.txt",true,true);//WriteCapturedDataToFile(bool bWriteToFile, QString qsFileName, bool bWriteHeaderInfo, bool bWriteCalibratedData)
ExampleDevicePluginObj.ConfigureHIDTriggers(false,false,3,1);//=DetectionMethod::MaskedValueChangedHigh);
ExampleDevicePluginObj.ConfigureHIDTriggers(true,true,3,1);//=DetectionMethod::MaskedValueChangedHigh);
//ConfigureHIDTriggers(bool bActivateJoystickTrigger = true, bool bActivateButtonTriggers = false, unsigned char cButtonMask = 255, DetectionMethod ButtonDetectDetectionMethod = DetectionMethod::MaskedValueChanged);
ExampleDevicePluginObj.ConfigureHIDFiltering(true, 1, true, 10);
//void ConfigureHIDFiltering(bool bActivateJoystickStabilisation, int nJoystickStabilisationThreshold, bool bActivateJoystickHistory, int nJoystickHistorySize);

ExampleDevicePluginObj.StartCaptureThread();
timer.timeout.connect(this, this.timerFunction);
timer.start(1000);
