var sScriptPath = BrainStim.getActiveDocumentFileLocation() + "/";
var dAppRootDir = new QDir(sScriptPath + "../");
var sSourcePath = dAppRootDir.absolutePath() + "/";
var sDestPath = sSourcePath;
var sSourceList = new CreateArray();
var sDestList = new CreateArray();

////////////////////////////////////////////////////////////////////////////
//Changes below here...
////////////////////////////////////////////////////////////////////////////
var sCopyrightString = "\"Copyright (C) 2016\"";

var sFileVersionMajor = 1;
var sFileVersionMinor = 1;
var sFileVersionRevision = 0;
var sFileVersionBuild = 1;

var sProductVersionMajor = 1;
var sProductVersionMinor = 1;
var sProductVersionRevision = 0;
var sProductVersionBuild = 1;

var sMinimalBrainStimVersion = "\"1.1.0.1\"";

sSourceList.push(sSourcePath + "defines.h");
sDestList.push(sDestPath + "defines.h");//0
//sSourceList.push(sSourcePath + "x.h");
//sDestList.push(sDestPath + "x.h");//1
//BrainStim.openFiles("",sSourceList);
///////////////////////Till here////////////////////////////////////////////

function tr(s) { return s; }

function CreateTimeStamp(sTime)
{
	var sep = "";
	var sTempTime = (sTime.getFullYear()-2000) + sep + (sTime.getMonth()+1) + sep + sTime.getDate() + sep + sTime.getHours() + sep + sTime.getMinutes() + sep + sTime.getSeconds();
	Log(sTempTime);
	return sTempTime;
}

function CreateArray(length) 
{
	var a = new Array(length || 0);
	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		for (var i = 0; i < length; i++) 
		{
			a[i] = CreateArray.apply(this, args);
		}
	}
	return a;
}

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function quotedString(sValue)
{
	return "\"" + sValue + "\"";
}

function CreateNewContent(strTemplate,changeSetArray)
{	
	var arrLines = strTemplate.split(/\r?\n|\r/);
	Log("  - Number of lines: " + arrLines.length);
	var nStartIndex = 0;
	var sSearchval = "";
	for(var nChanges=0;nChanges<changeSetArray[0].length;nChanges++)//For all changes
	{
		sSearchval = changeSetArray[0][nChanges];
		for(var nLines=0;nLines<arrLines.length;nLines++)//For all lines
		{
			if (arrLines[nLines].indexOf(sSearchval, nStartIndex) != -1) 
			{
				Log("    - Value(\"" + sSearchval + "\") found @ line:" + nLines);
				arrLines[nLines] = changeSetArray[1][nChanges];
				//Log(arrLines[nLines]);
			}
		}
	}
	strTemplate = arrLines.join("\n");
	return strTemplate;
}

function ProcessInFiles(strTemplate, strDestination, changeSetArr)
{
	var templateFile = new QFile(strTemplate);
	templateFile.open(QIODevice.OpenMode(QIODevice.ReadOnly) || QIODevice.OpenMode(QIODevice.Text));
	var inStream = new QTextStream(templateFile);
	var inData = new String();
	inData = inStream.readAll();
	templateFile.close();
	///////////////////////////////////////////////////////////////////////////////////////
	var outputFile = new QFile(strDestination);
	outputFile.open(QIODevice.OpenMode(QIODevice.WriteOnly) || QIODevice.OpenMode(QIODevice.Text));
	var outStream = new QTextStream(outputFile);
	inData = CreateNewContent(inData,changeSetArr);
	var outData = new QByteArray(inData);// or 'text'
	outputFile.write(outData);
	outputFile.close();
	templateFile = null;
	inStream = null;
	inData = null;
	outputFile = null;
	outStream = null;
	outData = null;
	return true;
}

function getFileName()
{
	var options = 0;
	//options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	//( parent, caption, dir, filter, selectedFilter, options)
	var fileName = QFileDialog.getOpenFileName(this,
				tr("QFileDialog::getOpenFileName()"),
				sScriptPath,
				tr("JavaScript Files (*.js)"),
				selectedFilter,
				QFileDialog.Options(options));
	if (fileName.length != 0)
		return fileName;
	else
		return "";
}

function CleanupScript()
{
	jsConfigurationFile = null;
	sTimeObject = null;
	CreateTimeStamp = null;
	tmpString = null;
	changeSet= null;
	ProcessInFiles = null;
	QByteArray.prototype.toString =null;
	CreateNewContent = null;
	CreateArray = null;
	sSourceList = null;
	getFileName = null;
	CleanupScript = null;
	tr = null;
	BrainStimInfo = null;
	quotedString = null;
	dAppRootDir = null;
	Log("\nScript ended");
	BrainStim.cleanupScript();
}

var tmpString = new String();
var sTimeObject = new Date();
var nFiles = sSourceList.length;
var changeSet;
var sComment = "   //Please do not edit this line manually, see PluginBinaryVersioning.qs (" + CreateTimeStamp(sTimeObject) + ").";
var bDoProcess = true;
var nChangeCounter;

if(bDoProcess)
{
	Log("Total " + nFiles + " file(s).");
	for(var nFileCounter=0;nFileCounter<nFiles;nFileCounter++)
	{
		changeSet = CreateArray(nFiles,2,0);
		nChangeCounter = 0;
		bDoProcess = false;
		if(nFileCounter == 0)//"defines.h"
		{		
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sFileVersionMajor + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sFileVersionMinor + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sFileVersionRevision + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sFileVersionBuild + sComment;
			nChangeCounter++;		
		
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sProductVersionMajor + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sProductVersionMinor + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sProductVersionRevision + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sProductVersionBuild + sComment;
			nChangeCounter++;		
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_LEGAL_COPYRIGHT";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sCopyrightString + sComment;
			nChangeCounter++;
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + sMinimalBrainStimVersion + sComment;
			nChangeCounter++;
			
			bDoProcess = true;
		}
		else
		{		
			bDoProcess = false;
		}
		
		if(bDoProcess)
		{
			Log("\n- File: " + sSourceList[nFileCounter]);
			ProcessInFiles(sSourceList[nFileCounter],sDestList[nFileCounter],changeSet[nFileCounter]);		
		}
	}
}
CleanupScript();
