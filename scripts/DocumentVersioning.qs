var sBrainStimDocPath = "../../../BrainStim_Documentation";
var fiBrainStimDocPath = new QFileInfo(sBrainStimDocPath);
sBrainStimDocPath = fiBrainStimDocPath.canonicalFilePath();
Log(sBrainStimDocPath);
fiBrainStimDocPath = null;
BrainStim.addScriptIncludePath(sBrainStimDocPath + "/scripts");
Include(sBrainStimDocPath + "/scripts/DocumentVersioningLib.qs");
	
/////////////////////////CONFIGURATION(BEGIN)////////////////////////////////////////////
_sPluginName = "<Rename>";
_sPluginDocRootFolderName = "<Rename>";
_sPluginVersion = "1.0.0.1";
_sDocVersion = "1.0.0.1";
var _sMinimalBrainStimVersion = "1.1.0.0";
var _sGitHubAccount = "svengijsen";

docVer_sSourcePath 				= BrainStim.getActiveDocumentFileLocation() + "/../" + _sPluginDocRootFolderName + "/";
docVer_sDestPath 				= docVer_sSourcePath;
docVer_sMainAuthor				= "Sven Gijsen";
docVer_sCurrentAuthoringMonth 	= "Juli 2015";
docVer_sComment 				= "Please do not edit the below line(s) manually, see DocumentVersioning.qs (" + CreateTimeStamp(sTimeObject) + ")";
docVer_sScriptComment  			= "//" + docVer_sComment + ", BrainStim version " + BrainStim_Settings_CurrentBrainStimReleaseString + ", Plugin version " + _sPluginVersion;
docVer_sHTMLComment  			= "<!-- " + docVer_sComment + ",BrainStim version " + BrainStim_Settings_CurrentBrainStimReleaseString + ", Plugin version " + _sPluginVersion + "-->";

//Default Document Editing
defaultDocumentEditing("../index.html", "index.html", 0);
defaultDocumentEditing("index.html", "index.html", 1);

//Special (non-default Document Editing)
sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-title-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"<title>" + _sPluginName + " v" + _sPluginVersion + ", a BrainStim Plugin</title>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-title-FB-end-->";

sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-doclink-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t<a href=\"" + _sPluginName + "/index.html\">documentation</a>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-doclink-FB-end-->";
	
sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-nameversion-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t" + _sPluginName + " (v" + _sPluginVersion + ")";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-nameversion-FB-end-->";

sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-doxylink-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t<a href=\"" + _sPluginName + "/Doxygen/index.html\">Doxygen documentation</a>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-doxylink-FB-end-->";

sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-brainstimversion-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t" + _sMinimalBrainStimVersion;
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-brainstimversion-FB-end-->";

sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-releaseslink-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t<a href=\"http://github.com/" + _sGitHubAccount + "/" + _sPluginName + "/releases/\" target=\"_blank\">";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-releaseslink-FB-end-->";
	
sSourceList.push(docVer_sSourcePath + "../index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-repolink-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t<a href=\"http://github.com/" + _sGitHubAccount + "/" + _sPluginName + "/\" target=\"_blank\">";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-repolink-FB-end-->";	

sSourceList.push(docVer_sSourcePath + "index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-title-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + "<title>" + _sPluginName + " v" + _sPluginVersion + "</title>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-title-FB-end-->";
	
sSourceList.push(docVer_sSourcePath + "index.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-default-header-FB-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
//"<title>" + _sPluginName + " v" + _sPluginVersion + "</title>";
"	<div class=\"doc-header\">\n" +
"		<div class=\"alert alert-info\" role=\"alert\">\n" +
"			<h1>" + _sPluginName + " Plugin v" + _sPluginVersion + "</h1>\n" +
"			<div class=\"row\">\n" +
"				<div class=\"col-md-4\"><p><strong>Document version: </strong>" + _sDocVersion + "</p></div>\n" +
"				<div class=\"col-md-4\"><p><strong>Author(s): </strong>" + docVer_sMainAuthor + "</p></div>\n" +
"				<div class=\"col-md-4\"><p><strong>Date: </strong>" + docVer_sCurrentAuthoringMonth + "</p></div>\n" +
"			</div>\n" +
"		</div>\n" +
"	</div>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-default-header-FB-end-->";	

sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhcp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"  <assistant>\n" +
		"		<title>" + _sPluginName + " plugin v" + _sPluginVersion + "</title>\n" +
		"    <applicationIcon>" + _sPluginName + "/images/logo.jpg</applicationIcon>\n" +
		"    <cacheDirectory>" + _sGitHubAccount + "/BrainStim/Plugins/" + _sPluginName + "</cacheDirectory>\n" +
		"    <startPage>qthelp://com." + _sGitHubAccount + ".brainstim/doc/plugins/" + _sPluginName + "/index.html</startPage>\n" +
		"    <aboutMenuText>\n" +
		"        <text>About " + _sPluginName + "</text>\n" +
		"    </aboutMenuText>\n" +
		"    <aboutDialog>\n" +
		"        <file>" + _sPluginName + "/additional/about.txt</file>\n" +
		"        <icon>" + _sPluginName + "/images/logo.jpg</icon>\n" +
		"    </aboutDialog>\n" +
		"    <enableDocumentationManager>false</enableDocumentationManager>\n" +
		"    <enableAddressBar>false</enableAddressBar>\n" +
		"    <enableFilterFunctionality>false</enableFilterFunctionality>\n" +
		"  </assistant>\n" +
		"  <docFiles>\n" +
		"  	<generate>\n" +
		"    	<file>\n" +
		"      	<input>" + _sPluginName + ".qhp</input>\n" +
		"        <output>" + _sPluginName + ".qch</output>\n" +
		"      </file>\n" +
		"    </generate>\n" +
		"    <register>\n" +
		"    	<file>" + _sPluginName + ".qch</file>\n" +
		"    </register>\n" +
		"  </docFiles>\n" +
		"	</QHelpCollectionProject>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-end-->";	

sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-namespace-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t<namespace>com.trolltech.brainstim.plugins." + _sPluginName + "</namespace>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-namespace-end-->";

sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-toc-root-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t\t<section title=\"Plugin: " + _sPluginName + "\" ref=\"./" + _sPluginName + "/index.html\">";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-toc-root-end-->";
	
sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-toc-scriptclass-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t\t\t<section title=\"" + _sPluginName + " &lt;Script Class&gt;\"  ref=\"./" + _sPluginName + "/Doxygen/index.html\">";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-toc-scriptclass-end-->";

sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-keywords-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t\t<keyword name=\"" + _sPluginName + "\" ref=\"./" + _sPluginName + "/index.html\"/>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-keywords-end-->";
	
sSourceList.push(docVer_sSourcePath + "../" + _sPluginName + ".qhp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qhelp-files-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
		"\t\t\t<file>bootstrap/css/*</file>\n" +
		"\t\t\t<file>bootstrap/fonts/*</file>\n" +
		"\t\t\t<file>bootstrap/js/*</file>\n" +
		"\t\t\t<file>google-code-prettify/*</file>\n" +
		"\t\t\t<file>css/*</file>\n" +
		"\t\t\t<file>" + _sPluginName + "/*</file>\n" +
		"\t\t\t<file>" + _sPluginName + "/additional/*</file>\n" +
		"\t\t\t<file>" + _sPluginName + "/images/*</file>\n" +
		"\t\t\t<file>" + _sPluginName + "/Doxygen/*</file>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qhelp-files-end-->";
/////////////////////////CONFIGURATION(END)//////////////////////////////////////////////

startDocumentEditing();
docVer_CleanupScript();
