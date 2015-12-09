var sBrainStimDocPath = "../../../BrainStim_Documentation";
var fiBrainStimDocPath = new QFileInfo(sBrainStimDocPath);
sBrainStimDocPath = fiBrainStimDocPath.canonicalFilePath();
Log(sBrainStimDocPath);
fiBrainStimDocPath = null;
BrainStim.addScriptIncludePath(sBrainStimDocPath + "/scripts");
Include("DoxygenBuildLib.qs");
var fileSource = sBrainStimDocPath + "/scripts/doxygen/PluginTemplate.cfg";

var BrainStim_Info = new BrainStim_Information();
var tmpStringList =  [];
var bFileProccessed;
var nCounter;
var bSkipStep;
var fileDest;
var changeSet;
var preFix = "-*/*-";
var postFix = "-*/*-";
var sBinairyPath = "doxygen";
var sDoxyFileDir = BrainStim.getActiveDocumentFileLocation() + "/";
var LastComponentVersion;
var ComponentName;
var bDoCleanup = false;
var sBinairySteps = 1;

for(nCounter=1;nCounter<=sBinairySteps;nCounter++)
{
	bFileProccessed = false;
	bSkipStep = false;
	
	Log("** STEP "  + nCounter + " **");
	
	changeSet = BrainStim_CreateArray(8,2);
	if(nCounter==1)
	{
		ComponentName = "<Rename>Plugin";		
		Log("Component Name: " + ComponentName);
		LastComponentVersion  = "1.0.0.1";
		Log("Last Component Version: " + LastComponentVersion);
		Log("\n");
		fileDest = ComponentName + ".cfg";
		changeSet[0][0] = preFix + "PROJECT_NAME" + postFix;
		changeSet[0][1] = "\"FireBird Client Device script class\"";
		changeSet[1][0] = preFix + "PROJECT_NUMBER" + postFix;
		changeSet[1][1] = "\"" + LastComponentVersion + "\"";
		changeSet[2][0] = preFix + "PROJECT_BRIEF" + postFix;
		changeSet[2][1] = "\"FireBird Client Device script class reference.\"";
		changeSet[3][0] = preFix + "PROJECT_LOGO" + postFix;
		changeSet[3][1] = "";//No quotes for empty!
		changeSet[4][0] = preFix + "OUTPUT_DIRECTORY" + postFix;
		changeSet[4][1] = "\"../<Rename>\"";//"\"../References/Script/<Rename>Plugin\"";
		changeSet[5][0] = preFix + "INPUT" + postFix;
		changeSet[5][1] = "../../<Rename>/<Rename>.h \\\n" +
					  "../../<Rename>/<Rename>.cpp \\\n";
		changeSet[6][0] = preFix + "QHP_NAMESPACE" + postFix;
		changeSet[6][1] = "BrainStim.doxygen.Project";	
		changeSet[7][0] = preFix + "PREDEFINED" + postFix; //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html
		changeSet[7][1] = "";		
	}	
	else
	{
		bSkipStep = true;
	}
	
	if(!bSkipStep)
	{
		GDCF_ReplaceInFiles(fileSource,fileDest,changeSet);
		//doxygen -w html header.html footer.html stylesheet.css <config_file>
		tmpStringList = [];//Reset list
		tmpStringList[0] = sDoxyFileDir + fileDest;
	}
	
	if(!bSkipStep)
	{
		if(DoxyBuildLib_ExecuteBinairy(sBinairyPath,tmpStringList))
		{
			Log("DoxyBuildLib_ExecuteBinairy started Successfully!");
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
			Log("DoxyBuildLib_ExecuteBinairy Failed!");
			DoxyBuildLib_CleanUpScript();
		}
	}
	else
	{
		Log("** STEP "  + nCounter + " ** SKIPPED");
		continue;
	}
}