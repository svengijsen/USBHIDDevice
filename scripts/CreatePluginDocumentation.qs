var sPluginName = "<Rename>";
var tmpString;
var InstallProcess = new QProcess();
var tmpByteArray = new QByteArray();
var tmpStringList =  [];
var tmpString = new String();
var tmpFile = new QFile();
var sBinairySteps;
var bDoCleanup;
var bFileProccessed;
var nCounter;
var bSkipStep;
var sScriptPath = BrainStim.getActiveDocumentFileLocation() + "/";
var sRootPath = sScriptPath + "../";
var sPluginProjectPath = sScriptPath + "../../";
var sBinairyPath;
var sBrainStimProjectDirectory = ToWindowsPath("../../../BrainStim");
var sQTDirectory = ToWindowsPath("E:/Libraries/Qt5.3.2_32bit/5.3/msvc2013_opengl/");

InstallProcess.setProcessChannelMode(QProcess.MergedChannels);

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function ToWindowsPath(path)
{
	return path.replace( /\//gi,"\\");//  first argument = /regex/ (g=all occurrences, i=case insensitive), second = <replace_string>
}

function CleanUpScript()
{
	Log("CleanUpScript started...");
	ConnectDisconnectScriptFunctions(false);
	ToWindowsPath=null;
	tr=null;
	tmpFile=null;
	tmpByteArray=null;
	tmpStringList=null;
	tmpString=null;
	ExecuteBinairy=null;
	QByteArray.prototype.toString=null;
	InstallProcess=null;
	ConnectDisconnectScriptFunctions=null;
	processFinished=null;
	DeleteFile=null;
	processReadyRead=null;
	getString = null;
	CleanUpScript=null;	
	Log("CleanUpScript exited successfully!");
	//Beep();
	BrainStim.cleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("Connecting Signal/Slots...");
		try 
		{	
			InstallProcess.readyRead.connect(this, this.processReadyRead);	
			InstallProcess["finished(int,QProcess::ExitStatus)"].connect(this, this.processFinished);	 
		} 
		catch (e) 
		{
			Log("<font color=\"red\">.*. Something went wrong connecting the Signal/Slots:</font>" + e);
		}		
	}
	else
	{
		Log("Disconnecting Signal/Slots...");
		try 
		{	
			InstallProcess.readyRead.disconnect(this, this.processReadyRead);
			InstallProcess.finished.disconnect(this, this.processFinished);	 
		} 
		catch (e) 
		{
			Log("<font color=\"red\">.*. Something went wrong disconnecting the Signal/Slots:</font>" + e);
		}		
	}	
}

function tr(s) { return s; }

function getString(title,text,defaultValue)
{
	tmpString = QInputDialog.getText(this, tr(title), tr(text), QLineEdit.Normal, tr(defaultValue), Qt.WindowFlags(0));
	if (tmpString != null && tmpString.length != 0)
	{
		return tr(tmpString);
	}
	else
	{	
		return tr("");
	}
}

function processReadyRead()
{
	tmpByteArray = InstallProcess.readAll();
	Log(tmpByteArray.toString());	
}

function processFinished()
{
	Log("processFinished() called!");
	Log("** STEP "  + nCounter + " ** PASSED!");
	bFileProccessed = true;
	if(bDoCleanup)
		CleanUpScript();
}

function DeleteFile(sPath)
{
	tmpFile.setFileName(sPath);
	if(tmpFile.exists())
	{
		tmpFile.remove();
		if(tmpFile.exists()==false)
			Log("Successfully deleted file: " + sPath);
	}
}

function ExecuteBinairy(binPath, argList)
{
	InstallProcess.start(binPath,argList,QIODevice.ReadWrite);// + "bin/qhelpgenerator", tmpStringList);
	var sCommand = binPath;
	for(var i=0;i<argList.length;i++)
		sCommand = sCommand + " " + argList[i];
	Log("\n\t>> " + sCommand + "\n");
	return InstallProcess.waitForStarted();
}

ConnectDisconnectScriptFunctions(true);
//Log(getString("This is an Title", "Some Text....","default x"));


//Name				Extension	Brief Description
//Qt Help Project			.qhp		The input file for the help generator consisting of the table of contents, indices and references to the actual documentation files (*.html); 
//							it also defines a unique namespace for the documentation.
//Qt Compressed Help		.qch		The output file of the help generator. This binary file contains all information specified in the help project file along with all compressed documentation files.
//Qt Help Collection Project	.qhcp		The input file for the help collection generator. It contains references to compressed help files which should be included in the collection; 
//							it also may contain other information for customizing Qt Assistant.
//Qt Help Collection			.qhc		The output of the help collection generator. This is the file QHelpEngine operates on. It contains references to any number of compressed help files as well as additional information, 
//							such as custom filters.
bDoCleanup = false;
sBinairySteps = 16;
sScriptPath = ToWindowsPath(sScriptPath);//Important!
sRootPath = ToWindowsPath(sRootPath);//Important!
sPluginProjectPath = ToWindowsPath(sPluginProjectPath);//Important!
//sQtDirectory = getString("Choose the Qt directory","Qt directory:",sQtDirectory_default);
//Log("Qt directory = " + sQtDirectory);
sBrainStimProjectDirectory = getString("Choose the BrainStim project directory","BrainStim project directory:",sBrainStimProjectDirectory);
Log("BrainStim project directory = " + sBrainStimProjectDirectory);
for(nCounter=1;nCounter<=sBinairySteps;nCounter++)
{
	bFileProccessed = false;
	bSkipStep = false;
	
	Log("** STEP "  + nCounter + " **, from " + sBinairySteps + " step(s)");
	if (nCounter==1)
	{
		bSkipStep = true;
		DeleteFile(sPluginProjectPath + "build/" + sPluginName + ".qhc");
	}
	if (nCounter==2)
	{
		bSkipStep = true;
		DeleteFile(sPluginProjectPath + "build/" + sPluginName + ".qch");
	}
	else if(nCounter==3)
	{
	//	qcollectiongenerator mycollection.qhcp -o mycollection.qhc
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhcp";
		tmpStringList[1] = "-o"
		tmpStringList[2] = sRootPath + sPluginName + ".qhc";
		sBinairyPath = sQTDirectory + "bin/qcollectiongenerator";
	}
	else if (nCounter==4)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sBrainStimProjectDirectory + "\\Install\\documents\\"  + sPluginName + ".qhc");
	}
	else if (nCounter==5)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sBrainStimProjectDirectory + "\\Install\\documents\\"  + sPluginName + ".qch");
	}
	else if (nCounter==6)
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==7)
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qch";
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==8)//Win32 - Release
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sRootPath + "build\\Win32\\Release\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==9)//Win32 - Release
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qch";
		tmpStringList[1] = sRootPath + "build\\Win32\\Release\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==10)//Win32 - Debug
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sRootPath + "build\\Win32\\Debug\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==11)//Win32 - Debug
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qch";
		tmpStringList[1] = sRootPath + "build\\Win32\\Debug\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==12)//X64 - Release
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sRootPath + "build\\x64\\Release\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==13)//X64 - Release
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qch";
		tmpStringList[1] = sRootPath + "build\\x64\\Release\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==14)//X64 - Debug
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sRootPath + "build\\x64\\Debug\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==15)//X64 - Debug
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sRootPath + sPluginName + ".qch";
		tmpStringList[1] = sRootPath + "build\\x64\\Debug\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==16)
	{
		//assistant -collectionFile MainDocumentation.qhc
		tmpStringList = [];//Reset list
		tmpStringList[0] = "-collectionFile";
		//tmpStringList[1] = sRootPath + sPluginName + ".qhc";
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\"  + sPluginName + ".qhc";
		sBinairyPath = "assistant";		
	}
	else
	{
		bSkipStep = true;
	}

	if(!bSkipStep)
	{
		if(ExecuteBinairy(sBinairyPath,tmpStringList))
		{
			Log("ExecuteBinairy started Successfully!");
			if(nCounter == sBinairySteps)
				bDoCleanup = true;
			while(!bFileProccessed)
			{
				BrainStim.processEvents();
				//Log("Waiting...");
			}
		}
		else
		{
			Log("<font color=\"red\">+++++++++++ExecuteBinairy Failed!++++++++++++++</font>");
			CleanUpScript();
			break;
		}
	}
	else
	{
		Log("<font color=\"green\">** STEP "  + nCounter + " ** SKIPPED</font>");
		if(nCounter == sBinairySteps)
			CleanUpScript();			
		continue;
	}
}
//@echo off
//CALL .\bin\assistant.exe -collectionFile .\documents\MainDocumentation.qhc -enableRemoteControl



