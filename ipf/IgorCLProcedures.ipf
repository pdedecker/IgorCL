#pragma rtGlobals=3		// Use modern global access method and strict wave access.

#pragma IndependentModule=IgorCLEditor

#include <Resize Controls>
#include <SaveRestoreWindowCoords>

constant IgorCLReadWrite = 1
constant IgorCLWriteOnly = 2
constant IgorCLReadOnly = 4
constant IgorCLUseHostPointer = 8
constant IgorCLIsLocalMemory = 16
constant IgorCLIsScalarArgument = 32

constant kUnsigned = 1
constant kInt8 = 2
constant kInt16 = 4
constant kInt32 = 8
constant kInt64 = 16
constant kFP32 = 32
constant kFP64 = 64
constant kIgorCLTypeWildCard = 128

strconstant ksTypeWildCard = "ICLT_"


Menu "Macros"
	"IgorCL Editor", /Q, SetupProcedureEditor()
End

Function SetupProcedureEditor()
	
	DoWindow /F IgorCLEditor
	if (V_flag != 1)
		string platforms = "\"" + GetOpenCLPlatforms() + "\""
		string deviceNames = "\"" + GetOpenCLDeviceNames(0) + "\""
		
		NewPanel /K=1/W=(150,50,1034,685) /N=IgorCLEditor
		GroupBox GBSourceCode,win=IgorCLEditor,pos={15,1},size={855,468},title="Source Code"
		GroupBox GBSourceCode,win=IgorCLEditor
		GroupBox GBSourceCode,win=IgorCLEditor
		GroupBox GBSourceCode,win=IgorCLEditor
		GroupBox GBSourceCode,userdata(ResizeControlsInfo)= A"!!,B)!!#66!!#De^]6aez!!#](Aon\"q7:^+SDfSN_Aon\"Qzzzzzzzzzzz!!#o2B4uAe7:^+SDfS`i"
		GroupBox GBSourceCode,userdata(ResizeControlsInfo) += A"B4uAezzzzzzzzzzz!!#u:DuaGl7:^+SDfSfqDu]k<zzzzzzzz"
		GroupBox GBSourceCode,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<!+]ecDfS0_FEDG<zzzzzzzzzzz!!!"
		DefineGuide EditorLeft={FL,20},EditorRight={FR,-20},EditorTop={FT,20},EditorBottom={FB,-175}
		NewNotebook /N=CodeEditor /F=0 /N=NB0 /W=(116,23,500,307)/FG=(EditorLeft,EditorTop,EditorRight,EditorBottom) /HOST=IgorCLEditor
		
		GroupBox GBCompileOutput,win=IgorCLEditor,pos={14,473},size={856,80},title="Compile output"
		GroupBox GBCompileOutput,win=IgorCLEditor
		GroupBox GBCompileOutput,win=IgorCLEditor
		GroupBox GBCompileOutput,win=IgorCLEditor
		GroupBox GBCompileOutput,userdata(ResizeControlsInfo)= A"!!,An!!#CQJ,hu<!!#?Yz!!#](Aon\"q6Z6g\\Bl%?VF`_SFFA-7XF8u:@zzzzzzzzz!!#o2B4uAe6Z6g\\Bl%?V"
		GroupBox GBCompileOutput,userdata(ResizeControlsInfo) += A"F`_SFFAcgcBQO4Szzzzzzzzz!!#?(FEDG<!,l^nBl%?VF`_SFFB!0tzzzzzzz"
		GroupBox GBCompileOutput,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<!,l^nBl%?VF`_SFF@'nfFDl!rzzzzzzzzz!!!"
		DefineGuide CompileOutputTop={FB,-145},CompileOutputBottom={FB,-90},CompileOutputLeft={EditorLeft, 0},CompileOutputRight={EditorRight,0}
		NewNotebook /F=0 /N=NBCompileOutput /W=(226,160,679,480)/FG=(CompileOutputLeft,CompileOutputTop,CompileOutputRight,CompileOutputBottom) /HOST=IgorCLEditor 
		Notebook IgorCLEditor#NBCompileOutput, changeableByCommandOnly=1
		
		PopupMenu PMCodeDestination,win=IgorCLEditor,pos={281,603},size={205,20},bodyWidth=150,title="Destination:",proc=PMCodeDestinationProc
		PopupMenu PMCodeDestination,win=IgorCLEditor,mode=1,popvalue="Save in text wave",value= #"\"Save in text wave;Compile for device;Copy to clipboard as Igor string;Copy to clipboard as C string;Copy Igor skeleton code to clipboard;\""
		PopupMenu PMCodeDestination,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,HGJ,htQ^]6_r!!#<Xz!!#o2B4uAezzzzzzzzzzzzzz!!#o2B4uAezz"
		PopupMenu PMCodeDestination,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		PopupMenu PMCodeDestination,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		SetVariable SVOutputWaveName,win=IgorCLEditor,pos={501,606},size={200,15},title="Output wave name:"
		SetVariable SVOutputWaveName,win=IgorCLEditor,value= _STR:"W_IgorCLProgram"
		SetVariable SVOutputWaveName,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,I`J,htRJ,hr-!!#<(z!!#o2B4uAezzzzzzzzzzzzzz!!#o2B4uAezz"
		SetVariable SVOutputWaveName,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		SetVariable SVOutputWaveName,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		PopupMenu PMPlatform,win=IgorCLEditor,pos={15,560},size={243,20},bodyWidth=200,title="Platform:"
		PopupMenu PMPlatform,win=IgorCLEditor,mode=1,value= #platforms
		PopupMenu PMPlatform,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,B)!!#Cq!!#B-!!#<Xz!!#](Aon\"Qzzzzzzzzzzzzzz!!#](Aon\"Qzz"
		PopupMenu PMPlatform,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		PopupMenu PMPlatform,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		PopupMenu PMDevice,win=IgorCLEditor,pos={287,560},size={335,20},bodyWidth=300,title="Device:"
		PopupMenu PMDevice,win=IgorCLEditor,mode=1,value= #deviceNames
		PopupMenu PMDevice,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,HJJ,htG!!#BaJ,hm.z!!#o2B4uAezzzzzzzzzzzzzz!!#o2B4uAezz"
		PopupMenu PMDevice,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		PopupMenu PMDevice,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		Button BTTestCompile,win=IgorCLEditor,pos={717,563},size={150,20},proc=BTTestCompileProc,title="Test Compile"
		Button BTTestCompile,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,JD5QF1\\^]6_;!!#<Xz!!#o2B4uAezzzzzzzzzzzzzz!!#o2B4uAezz"
		Button BTTestCompile,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		Button BTTestCompile,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		Button BTSaveOrCompile,win=IgorCLEditor,pos={716,604},size={150,20},title="Save as text",proc=BTSaveOrCompileProc
		Button BTSaveOrCompile,win=IgorCLEditor,userdata(ResizeControlsInfo)= A"!!,JD!!#D'!!#A%!!#<Xz!!#o2B4uAezzzzzzzzzzzzzz!!#o2B4uAezz"
		Button BTSaveOrCompile,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzz!!#?(FEDG<zzzzzzzzzzz"
		Button BTSaveOrCompile,win=IgorCLEditor,userdata(ResizeControlsInfo) += A"zzz!!#?(FEDG<zzzzzzzzzzzzzz!!!"
		
		SetWindow IgorCLEditor,hook(ResizeControls)=ResizeControls#ResizeControlsHook
		SetWindow IgorCLEditor,userdata(ResizeControlsInfo)= A"!!*'\"z!!#Dm!!#D.^]4?7zzzzzzzzzzzzzzzzzzzz"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzzzzzzzzzzzzzzz"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfo) += A"zzzzzzzzzzzzzzzzzzz!!!"
		SetWindow IgorCLEditor,userdata(ResizeControlsGuides)=  "EditorLeft;EditorRight;EditorTop;EditorBottom;CompileOutputTop;CompileOutputBottom;CompileOutputLeft;CompileOutputRight;"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfoEditorLeft)= A":-hTC3^mRnFDl1nAS-$G=%Q.J8S;s^6VKpZBln964&f?Z764FiATBk':Jsbf:JOkT9KFjh:et\"]<(Tk\\3\\iBN7o`,K756hm9KPaE8OQ!&3]g5.9MeM`8Q88W:-(-a3r"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfoEditorRight)= A":-hTC3^mRnFDl1tBkM+$4',!K3_<t&E^NMhA8-.(E][6':dmEFF(KAR85E,T>#.mm5tj<n4&A^O8Q88W:-(?m1cn3-8OQ!%3_!\"/7o`,K75?nc;FO8U:K'ha8P`)B/M]\"A"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfoEditorTop)= A":-hTC3^mRnFDl2!Df@d>8PV<<B5_fX9LWK+FDl1]<*<$d3`U64E]Zff;Ft%f:/jMQ3\\`]m:K'ha8P`)B1,(d[<CoSI0fhd'4%E:B6q&jl4&SL@:et\"]<(Tk\\3\\iBN"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfoEditorBottom)= A":-hTC3^mRnFDl1dDfg)>D*(fj:-(sZDfS3=7:^+SDfRp==\\qOJ<HD_l4%N.F8Qnnb<'a2=0fr3-;b9q[:JNr-2D@3_<CoSI0fhcj4%E:B6q&jl4&SL@:et\"]<(Tk\\3\\<'F2*1"
		SetWindow IgorCLEditor,userdata(ResizeControlsInfoCompileOutput)= A":-hTC3^[h&E+s-\":N^buF`^MqB4uBK=%Q.J8S;s^6VKpZBln964&f?Z764FiATBk':Jsbf:JOkT9KFjh:et\"]<(Tk\\3]K#S4%E:B6q&gk7:^+SDfS`iB4uBK7o`,K75?nc;FO8U:K'ha8P`)B0KT"
	endif
End

Function PMCodeDestinationProc(pa) : PopupMenuControl
	STRUCT WMPopupAction &pa

	switch( pa.eventCode )
		case 2: // mouse up
			Variable popNum = pa.popNum
			String popStr = pa.popStr
			
			string windowName = pa.win
			SetVariable SVOutputWaveName, win=$windowName, disable=0
			StrSwitch (popStr)
				case "Save in text wave":
					Button BTSaveOrCompile,win=$windowName, title="Save as text"
					break
				case "Compile for device":
					Button BTSaveOrCompile,win=$windowName, title="Compile and save"
					break
				case "Copy to clipboard as Igor string":
				case "Copy to clipboard as C string":
				case "Copy Igor skeleton code to clipboard":
					Button BTSaveOrCompile,win=$windowName, title="Copy to clip"
					SetVariable SVOutputWaveName, win=$windowName, disable=1
					break
				default:
					Abort "Unknown destination type"
					break
			EndSwitch
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End

Function BTTestCompileProc(ba) : ButtonControl
	STRUCT WMButtonAction &ba

	switch( ba.eventCode )
		case 2: // mouse up
			// click code here
			string windowName = ba.win
			
			ControlInfo /W=$windowName PMCodeDestination
			string destinationMode = S_Value
			
			ControlInfo /W=$windowName SVOutputWaveName
			string outputWaveName = S_Value
			
			ControlInfo /W=$windowName PMPlatform
			variable platformIndex = V_Value - 1
			
			ControlInfo /W=$windowName PMDevice
			variable deviceIndex = V_Value - 1
			
			string fullName = windowName + "#" + "CodeEditor"
			Notebook $fullName getData=2
			string sourceText = S_Value		
			sourceText = ReplaceString("\r", sourceText, "\n")
			variable hasWildcards = HasTypeWildcards(sourceText)
			sourceText = PerformTypeSubstitutions(sourceText)
			
			KillWaves /Z W_IgorCLCompileDummy
			IgorCLCompile /PLTM=(platformIndex) /DEV=(deviceIndex) /DEST=W_IgorCLCompileDummy /Z sourceText
			KillWaves /Z W_IgorCLCompileDummy
			
			string buildLog = S_BuildLog
			variable err = V_flag
			Notebook $windowName#NBCompileOutput selection={startOfFile, endOfFile}	
			if (err == 0)
				string successText = "COMPILED OK"
				if (hasWildcards)
					successText += " using float for all type wildcards"
				endif
				NoteBook $windowName#NBCompileOutput text=(successText + "\r") + buildLog,textRGB=(0,0,0)
			else
				NoteBook $windowName#NBCompileOutput text=buildLog,textRGB=(65535,0,0)
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End

Function BTSaveOrCompileProc(ba) : ButtonControl
	STRUCT WMButtonAction &ba

	switch( ba.eventCode )
		case 2: // mouse up
			// click code here
			string windowName = ba.win
			
			ControlInfo /W=$windowName PMCodeDestination
			string destinationMode = S_Value
			
			ControlInfo /W=$windowName SVOutputWaveName
			string outputWaveName = S_Value
			
			ControlInfo /W=$windowName PMPlatform
			variable platformIndex = V_Value - 1
			
			ControlInfo /W=$windowName PMDevice
			variable deviceIndex = V_Value - 1
			
			string fullName = windowName + "#" + "CodeEditor"
			Notebook $fullName getData=2
			string sourceText = S_Value		
			sourceText = ReplaceString("\r", sourceText, "\n")
			
			StrSwitch (destinationMode)
				case "Save in text wave":
					SaveCodeAsTextWave(sourceText, outputWaveName)
					break
				case "Compile for device":
					CompileCodeToBinary(sourceText, platformIndex, deviceIndex, outputWaveName)
					break
				case "Copy to clipboard as Igor string":
					CopyCodeToIgorString(sourceText)
					break
				case "Copy to clipboard as C string":
					CopyCodeToCString(sourceText)
					break
				default:
				case "Copy Igor skeleton code to clipboard":
					CopyCodeToIgorSkeleton(sourceText)
					break
					Abort "Unknown destination type"
					break
			EndSwitch
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End

Function SaveCodeAsTextWave(sourceText, textWaveName)
	String sourceText, textWaveName
	sourceText = ReplaceString("\r\n", sourceText, "\n")
	sourceText = ReplaceString("\r", sourceText, "\n")
	sourceText = PerformTypeSubstitutions(sourceText)
	Make /N=1 /T /O $textWaveName
	wave /T textWave = $textWaveName
	textWave[0] = sourceText
End

Function CompileCodeToBinary(sourceText, platformIndex, deviceIndex, binaryWaveName)
	string sourceText
	variable platformIndex, deviceIndex
	string binaryWaveName
	
	KillWaves /Z $binaryWaveName
	IgorCLCompile /PLTM=(platformIndex) /DEV=(deviceIndex) /DEST=$binaryWaveName sourceText
End

Function /S CodeToIgorString(sourceText, [M_TypeSubstitutions])
	string sourceText
	wave /T M_TypeSubstitutions
	
	// replace all possible line endings with '\n'
	sourceText = ReplaceString("\r\n", sourceText, "\n")
	sourceText = ReplaceString("\r", sourceText, "\n")
	
	sourceText = PerformTypeSubstitutions(sourceText, M_TypeSubstitutions =M_TypeSubstitutions)
	
	variable sourceLen = strlen(sourceText)
	string igorCodeString = "string igorCLCode = \"\"\r"
	
	variable nLines = ItemsInList(sourceText, "\n")
	variable maxLineLength = 300, chunkLength, strIndex, i
	for (i = 0; i < nLines; i+=1)
		string thisLine = StringFromList(i, sourceText, "\n") + "\\n"
		strIndex = 0
		do
			chunkLength = min(strlen(thisLine) - strIndex, maxLineLength)
			igorCodeString += "igorCLCode += \"" + thisLine[strIndex, strIndex + chunkLength - 1] + "\"\r"
			strIndex += chunkLength
		while (strIndex < strlen(thisLine))
	endfor
	
	return igorCodeString
End

Function CopyCodeToIgorString(sourceText)
	string sourceText
	
	PutScrapText CodeToIgorString(sourceText)
End

Function CopyCodeToCString(sourceText)
	string sourceText
	
	// replace all possible line endings with '\n'
	sourceText = ReplaceString("\r\n", sourceText, "\n")
	sourceText = ReplaceString("\r", sourceText, "\n")
	sourceText = PerformTypeSubstitutions(sourceText)
	variable sourceLen = strlen(sourceText)
	
	string igorCodeString = "const char* kernelSource = \"\"\r"
	
	variable strIndex = 0, nextLoc, maxLineLength = 350
	string thisLine
	for ( ; strIndex < sourceLen; )
		nextLoc = StrSearch(sourceText, "\n", strIndex)
		if (nextLoc == -1)
			nextLoc = sourceLen - 1
		endif
		if (nextLoc - strIndex + 1 > maxLineLength)
			nextLoc = strIndex + maxLineLength - 1
		endif
		thisLine = sourceText[strIndex, nextLoc]
		thisLine = ReplaceString("\n", thisLine, "\\n")
		igorCodeString += "\"" + thisLine + "\"\r"
		strIndex = nextLoc + 1
	endfor
	igorCodeString += ";"
	
	PutScrapText igorCodeString
End

Function HasTypeWildCards(sourceText)
	string sourceText
	
	wave /T W_AllTypeWildCards = FindTypeWildcards(sourceText)
	return (DimSize(W_AllTypeWildCards, 0) > 0)
End

Function /S PerformTypeSubstitutions(sourceText, [M_TypeSubstitutions])
	string sourceText
	wave /T/Z M_TypeSubstitutions
	
	wave /T W_AllTypeWildCards = FindTypeWildcards(sourceText)
	
	// perform any supplied type substitutions
	variable i
	if (!ParamIsDefault(M_TypeSubstitutions) || WaveExists(M_TypeSubstitutions))
		for (i = 0; i < DimSize(M_TypeSubstitutions, 0); i+=1)
			sourceText = "#define " + M_TypeSubstitutions[i][0] + " " + M_TypeSubstitutions[i][1] + "\n" + sourceText
			FindValue /TEXT=M_TypeSubstitutions[i][0] /TXOP=4 W_AllTypeWildCards
			if (V_value == -1)
				Abort "Invalid type wildcard"
			endif
			DeletePoints V_value, 1, W_AllTypeWildCards
		endfor
	endif
	
	// everything that is left gets a default type
	for (i = 0; i < DimSize(W_AllTypeWildCards, 0); i+=1)
		sourceText = "#define " + W_AllTypeWildCards[i] + " float\t\t// placeholder type assigned by IgorCL\n" + sourceText
	endfor
	
	return sourceText
End

Function CopyCodeToIgorSkeleton(sourceText)
	string sourceText
	
	PutScrapText CreateSkeletonCode(sourceText)
End

Function /S GetOpenCLPlatforms()
	string platforms = ""
	
	IgorCLInfo
	wave /T M_OpenCLPlatforms
	
	variable i
	for (i = 0; i < DimSize(M_OpenCLPlatforms, 1); i+=1)
		platforms += M_OpenCLPlatforms[0][i] + ";"
	endfor
	
	return platforms
End

Function /S GetOpenCLDeviceNames(platformIndex)
	variable platformIndex
	string devices = ""
	
	IgorCLInfo
	string deviceWaveName = "M_OpenCLDevices" + num2istr(platformIndex)
	wave /T M_devices = $deviceWaveName
	
	variable i
	for (i = 0; i < DimSize(M_devices, 1); i+=1)
		devices += M_devices[1][i] + ";"
	endfor
	
	return devices
End

Function /S CreateSkeletonCode(sourceText)
	string sourceText
	
	string skeletonCode = ""
	
	// add constant definition
	skeletonCode += "constant IgorCLExecReadWrite = 1\r"
	skeletonCode += "constant IgorCLExecWriteOnly = 2\r"
	skeletonCode += "constant IgorCLExecReadOnly = 4\r"
	skeletonCode += "constant IgorCLExecUseHostPointer = 8\r"
	skeletonCode += "constant IgorCLExecIsLocalMemory = 16\r"
	skeletonCode += "constant IgorCLExecIsScalarArgument = 32\r"
	skeletonCode += "\r"
	
	wave /T W_Signatures = ExtractFunctionSignatures(sourceText)
	variable nSignatures = DimSize(W_Signatures, 0)
	variable i, paramIndex, haveAnyTypeWildCards = 1
	STRUCT ParameterStruct paramStruct
	for (i = 0; i < nSignatures; i+=1)
		wave /T W_NameAndParams = ExtractFunctionNameAndParams(W_Signatures[i])
		string functionName = W_NameAndParams[0]
		variable nParams = DimSize(W_NameAndParams, 0) - 1
		variable haveTypeWildCardsInThisFunction = 0
		// function declaration
		skeletonCode += "Function " + functionName + "("
		// declare params
		string paramDeclarations = ""
		// first we have the platform and device params
		skeletonCode += "platformIndex, deviceIndex, "
		paramDeclarations += "variable platformIndex		// OpenCL platform to use\rvariable deviceIndex		// device to calculate on\r"
		// then the kernel-specific params
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (paramStruct.isLocal)
				continue	// assume that the local memory sizes will be calculated in the function by the user
			endif
			if (paramStruct.type == kIgorCLTypeWildCard)
				haveTypeWildCardsInThisFunction = 1
			endif
			skeletonCode += ParamNameToIgorName(paramStruct) + ", "
			string paramType
			if (!paramStruct.isPointer)
				 paramType = "variable "
			else
				paramType = "wave "
			endif
			paramDeclarations += paramType + ParamNameToIgorName(paramStruct)
			// comment with more information
			if (paramStruct.isLocal)
				paramDeclarations += "\t\t// " + "required size of this local memory in bytes\r"
			elseif (!paramStruct.isPointer)
				paramDeclarations += "\t\t// " + "value of this argument to the kernel\r"
			elseif (paramStruct.isPointer && (paramStruct.type == kIgorCLTypeWildCard))
				paramDeclarations += "\t\t// " + "wave of any non-complex numeric type supported by the hardware\r"
			else
				paramDeclarations += "\t\t// " + "wave of type " + TypeIDToTypeName(paramStruct.type) + "\r"
			endif
		endfor
		skeletonCode = skeletonCode[0, strlen(skeletonCode) - 3]	// remove trailing comma and space
		skeletonCode += ")\r"
		skeletonCode += paramDeclarations + "\r"
		
		// allocate local variables
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (paramStruct.isLocal)
				skeletonCode += "variable " + ParamNameToIgorName(paramStruct) + " = 		// calculate size of this local memory here (in bytes!) \r"
			endif
		endfor
		skeletonCode += "\r"
		
		// make waves for the global and workgroup size
		skeletonCode += "// note: you can call the IgorCLInfo operation to adjust these settings to the device properties.\r"
		skeletonCode += "Make /FREE/I/U/N=3 W_GlobalSize, W_WorkGroupSize\r"
		skeletonCode += "W_GlobalSize[0] = 		// fill in based on your needs\r"
		skeletonCode += "W_GlobalSize[1] = 		// fill in based on your needs\r"
		skeletonCode += "W_GlobalSize[2] = 		// fill in based on your needs\r"
		skeletonCode += "W_WorkGroupSize[0] = 		// fill in based on your needs\r"
		skeletonCode += "W_WorkGroupSize[1] = 		// fill in based on your needs\r"
		skeletonCode += "W_WorkGroupSize[2] = 		// fill in based on your needs\r"
		skeletonCode += "\r"
		
		// make a memflags wave and set values as needed
		skeletonCode += "Make /I/U/FREE/N=(" + num2str(nParams) + ") W_MemFlags = 0\r"
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (paramStruct.isLocal)
				skeletonCode += "W_MemFlags[" + num2istr(paramIndex) + "] = IgorCLExecIsLocalMemory\r"
			endif
			if (!paramStruct.isPointer)
				skeletonCode += "W_MemFlags[" + num2istr(paramIndex) + "] = IgorCLExecIsScalarArgument\r"
			endif
			if (paramStruct.isGlobal)
				skeletonCode += "W_MemFlags[" + num2istr(paramIndex) + "] = 0		// set to IgorCLExecUseHostPointer if the device shares host memory \r"
			endif
		endfor
		skeletonCode += "\r"
		
		skeletonCode += "// No need to edit this function below this line\r\r"
		
		// if we have found type wildcards, edit the source code to reflect the actual types
		skeletonCode += "string clSourceCode = IgorCLSource()\r"
		if (haveTypeWildCardsInThisFunction)
			variable wildCardIndex = 0
			string wildCardStr = ""
			for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
				ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
				if (paramStruct.type != kIgorCLTypeWildCard)
					continue
				endif
				if (!paramStruct.isPointer || !paramStruct.isGlobal) // safety check
					Abort "Internal error: wildcards only for __global* memory"
				endif
				
				skeletonCode += "clSourceCode = ReplaceString(\"#define " + paramStruct.typeWildcardName + " float		// placeholder type assigned by IgorCL\\n\", clSourceCode, \"\")\r"
				skeletonCode += "clSourceCode = ReplaceString(\"" + paramStruct.typeWildcardName + "\", clSourceCode, WaveTypeToCType(" + ParamNameToIgorName(paramStruct) + "))\r"
			endfor
			skeletonCode += "\r"
		endif
		
		// add code to check the wave types
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (paramStruct.isPointer && paramStruct.isGlobal && (paramStruct.type != kIgorCLTypeWildCard))
				skeletonCode += "if (WaveType(" + ParamNameToIgorName(paramStruct) + ") != " + num2istr(IgorCLTypeToIgorWaveType(paramStruct.type)) + ")\r"
				skeletonCode += "Abort \"invalid wave type passed for " + ParamNameToIgorName(paramStruct) + "!\"\r"
				skeletonCode += "endif\r"
			endif
		endfor
		skeletonCode += "\r"
		
		// make waves needed for local memory or scalar arguments
		string wName
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (!paramStruct.isLocal && paramStruct.isPointer)
				continue
			endif
			
			skeletonCode += GenWaveMake(paramStruct) + "W_" + paramStruct.name + " = " + paramStruct.name + "\r"
		endfor
		skeletonCode += "\r"
		
		// generate the command for execution
		string execCmd
		string formatStr = "IgorCL /PLTM=(platformIndex) /DEV=(deviceIndex) /SRCT=clSourceCode /GSZE={W_GlobalSize[0],W_GlobalSize[1],W_GlobalSize[2]} /WGRP={W_WorkGroupSize[0], W_WorkGroupSize[1], W_WorkGroupSize[2]} /MFLG=W_MemFlags /KERN=\"%s\" "
		sprintf execCmd, formatStr, functionName
		for (paramIndex = 0; paramIndex < nParams; paramIndex += 1)
			ParseParam(W_NameAndParams[paramIndex + 1], paramStruct)
			if (paramStruct.isLocal || !paramStruct.isPointer)
				execCmd += "W_" + ParamNameToIgorName(paramStruct)
			else
				execCmd += ParamNameToIgorName(paramStruct)
			endif
			if (paramIndex != nParams - 1)
				execCmd += ", "
			endif
		endfor
		
		skeletonCode += execCmd + "\r\r"
		skeletonCode += "End\r\r"
		if (haveTypeWildCardsInThisFunction)
			haveAnyTypeWildCards = 1
		endif
	endfor
	
	// add the source code
	skeletonCode += "Static Function /S IgorCLSource()\r"
	skeletonCode += CodeToIgorString(sourceText)
	skeletonCode += "return igorCLCode\rEnd\r\r"
	
	// add the WaveTypeToCType function
	if (haveAnyTypeWildCards)
		skeletonCode += WaveTypeToCTypeFunction()
		skeletonCode += "\r"
	endif
	
	skeletonCode = IndentIgorCode(skeletonCode)
	return skeletonCode
End

Function /S WaveTypeToCTypeFunction()
	string code = ""
	code += "Static Function /S WaveTypeToCType(w)\r"
	code += "wave w\r\r"
	code += "variable type = WaveType(w)\r"
	code += "string typeStr = \"\"\r"
	code += "variable isUnsigned = type & 0x40\r"
	code += "type = type & ~0x40\r"
	code += "switch(type)\r"
	code += "case 0x2:\rreturn \"float\"\rbreak\r"
	code += "case 0x4:\rreturn \"double\"\rbreak\r"
	code += "case 0x8:\rtypeStr = \"char\"\rbreak\r"
	code += "case 0x10:\rtypeStr = \"short\"\rbreak\r"
	code += "case 0x20:\rtypeStr = \"int\"\rbreak\r"
	code += "default:\rAbort \"non-supported input or output wave type\"\rbreak\r"
	code += "EndSwitch\r\r"
	code += "if (isUnsigned)\r"
	code += "typeStr = \"unsigned \" + typeStr\r"
	code += "endif \r\r"
	code += "return typeStr\r"
	code += "End\r"
	
	return code
End

Function IgorCLTypeToIgorWaveType(clType)
	variable clType
	
	variable igorType
	variable isUnsigned = clType & kUnsigned
	clType = clType & ~kUnsigned
	
	switch (clType)
		case kFP32:
			return 0x2
			break
		case kFP64:
			return 0x4
			break
		case kInt8:
			igorType = 0x8
			break
		case kInt16:
			igorType = 0x10
			break
		case kInt32:
			igorType = 0x20
			break
		default:
			Abort "unknown cl type passed to  IgorCLTypeToIgorWaveType"
			break
	endswitch
	
	if (isUnsigned)
		igorType = igorType | 0x40
	endif
	
	return igorType
End

Function /S WaveTypeToIgorCLTypeFunction()
	string code = ""
	code += "Function WaveTypeToIgorCLType(w)\r"
	code += "wave w\r\r"
	code += "variable type = WaveType(w)\r"
	code += "variable isUnsigned = type & 0x40\r"
	code += "type = type & ~0x40\r"
	code += "switch(type)\r"
	code += "case 0x2:\rreturn " + num2str(kFP32) + "\rbreak\r"
	code += "case 0x4:\rreturn " + num2str(kFP64) + "\rbreak\r"
	code += "case 0x8:\rtype = " + num2str(kInt8) + "\rbreak\r"
	code += "case 0x10:\rtype = " + num2str(kInt16) + "\rbreak\r"
	code += "case 0x20:\rtype = " + num2str(kInt32) + "\rbreak\r"
	code += "default:\rAbort \"non-supported input or output wave type\"\rbreak\r"
	code += "EndSwitch\r\r"
	code += "if (isUnsigned)\r"
	code += "type = type | " + num2str(kUnsigned) + "\r"
	code += "endif \r\r"
	code += "return type\r"
	code += "End\r"
	
	return code
End

Structure ParameterStruct
	string name
	char isPointer
	char isGlobal
	char isLocal
	int32 type
	string typeWildcardName
EndStructure

Function /S ParamNameToIgorName(paramStruct)
	STRUCT ParameterStruct &paramStruct
	if (paramStruct.isLocal || !paramStruct.isPointer)
		return paramStruct.name
	else
		return "W_" + paramStruct.name
	endif
End

Function /WAVE ExtractFunctionSignatures(sourceText)
	string sourceText
	
	Make /T/N=0 /O W_FunctionSignatures
	
	string sourceNoWhiteSpace = ReplaceString(" ", sourceText, "")
	sourceNoWhiteSpace = ReplaceString("\n", sourceNoWhiteSpace, "")
	sourceNoWhiteSpace = ReplaceString("\t", sourceNoWhiteSpace, "")
	
	variable startOfFunction = 0
	variable functionIndex, parenLoc = 0
	for (functionIndex = 0; ; functionIndex += 1)
		startOfFunction = strsearch(sourceNoWhiteSpace, "kernelvoid", parenLoc)
		if (startOfFunction == -1)
			return W_FunctionSignatures
		endif
		
		startOfFunction += strlen("kernelvoid")
		parenLoc = strsearch(sourceNoWhiteSpace, "(", startOfFunction)
		parenLoc = FindParenIndex(sourceNoWhiteSpace, parenLoc + 1)
		if (parenLoc == -1)
			Abort "missing paren"
		endif
		
		Redimension /N=(functionIndex + 1) W_FunctionSignatures
		W_FunctionSignatures[functionIndex] = sourceNoWhiteSpace[startOfFunction, parenLoc]
	endfor
End

Function /WAVE ExtractFunctionNameAndParams(signature)
	string signature
	
	string functionName
	string parameters
	
	variable parenIndex = strsearch(signature, "(", 0)
	functionName = signature[0, parenIndex - 1]	// now we have the function name
	variable startOfParams = parenIndex + 1
	variable endOfParams = FindParenIndex(signature, startOfParams) - 1
	parameters = signature[startOfParams, endOfParams]
	
	variable nParams = ItemsInList(parameters, ",")
	Make /T/N=(nParams + 1) /FREE W_NameAndParams
	W_NameAndParams[0] = functionName
	W_NameAndParams[1, ] = StringFromList(p - 1, parameters, ",")
	
	return W_NameAndParams
End

Function ParseParam(paramStr, paramStruct)
	string paramStr
	STRUCT ParameterStruct &paramStruct
	
	paramStruct.isGlobal = 0
	paramStruct.isLocal = 0
	paramStruct.isPointer = (GrepString(paramStr, "\\*") != 0)
	paramStr = ReplaceString("*", paramStr, "")
	
	// replace namespace qualifiers
	if (StringMatch(paramStr, "__global*") || StringMatch(paramStr, "global*"))
		paramStruct.isGlobal = 1
		paramStr = paramStr[strsearch(paramStr, "global", 0) + strlen("global"), strlen(paramStr) - 1]
	endif
	if (StringMatch(paramStr, "__local*") || StringMatch(paramStr, "local*"))
		paramStruct.isLocal = 1
		paramStr = paramStr[strsearch(paramStr, "local", 0) + strlen("local"), strlen(paramStr) - 1]
	endif
	
	string parameterName = ""
	string possibleTypeWildcardName = ""
	paramStruct.type = ExtractType(paramStr, parameterName, possibleTypeWildcardName)
	if (paramStruct.type == kIgorCLTypeWildCard)
		paramStruct.typeWildcardName = possibleTypeWildcardName
	endif
	// wildcard types are only allowed for global pointers
	if ((paramStruct.type == kIgorCLTypeWildCard) && (!paramStruct.isPointer || !paramStruct.isGlobal))
		Abort "wildcard types are only supported for __global* parameters"
	endif
	paramStruct.name = parameterName
End

Function ExtractType(startsWithTypeStr, strWithNoType, typeWildcardName)
	string startsWithTypeStr
	string& strWithNoType
	string& typeWildcardName
	
	typeWildcardName = ""
	variable isUnsigned
	
	// floating point?
	if (StringMatch(startsWithTypeStr, "double*"))
		strWithNoType = startsWithTypeStr[strlen("double"), strlen(startsWithTypeStr) - 1]
		return kFP64
	endif
	if (StringMatch(startsWithTypeStr, "float*"))
		strWithNoType = startsWithTypeStr[strlen("float"), strlen(startsWithTypeStr) - 1]
		return kFP32
	endif
	
	// IgorCL type wildcard?
	string possibleTypeWildcard = ExtractTypeWildcardName(startsWithTypeStr, 0)
	if (strlen(possibleTypeWildcard) != 0)
		strWithNoType = startsWithTypeStr[strlen(possibleTypeWildcard), strlen(startsWithTypeStr) - 1]
		typeWildcardName = possibleTypeWildcard
		return kIgorCLTypeWildCard
	endif
	
	// unsigned type?
	if (StringMatch(startsWithTypeStr, "unsigned*"))
		startsWithTypeStr = startsWithTypeStr[strlen("unsigned"), strlen(startsWithTypeStr) - 1]
		isUnsigned = 1
	endif
	
	// must be integer
	variable type
	if (StringMatch(startsWithTypeStr, "char*"))
		strWithNoType = startsWithTypeStr[strlen("char"), strlen(startsWithTypeStr) - 1]
		type = kInt8
	elseif (StringMatch(startsWithTypeStr, "short*"))
		strWithNoType = startsWithTypeStr[strlen("short"), strlen(startsWithTypeStr) - 1]
		type = kInt16
	elseif (StringMatch(startsWithTypeStr, "int*"))
		strWithNoType = startsWithTypeStr[strlen("int"), strlen(startsWithTypeStr) - 1]
		type = kInt32
	elseif (StringMatch(startsWithTypeStr, "long*"))
		strWithNoType = startsWithTypeStr[strlen("long"), strlen(startsWithTypeStr) - 1]
		type = kInt64
	else
		Abort "unknown type"
	endif
	
	if (isUnsigned)
		type += kUnsigned
	endif
	
	return type
End

Function /S GenWaveMake(paramStruct)
	STRUCT ParameterStruct &paramStruct
	
	if (paramStruct.isLocal)
		return "Make /FREE/N=1/I/U "
	endif
	
	variable type = paramStruct.type
	string makeCmd = "Make /FREE/N=1"
	if (type & kUnsigned)
		makeCmd += "/U"
	endif
	type = type & ~kUnsigned
	
	switch (type)
		case kInt8:
			makeCmd += "/B"
			break
		case kInt16:
			makeCmd += "/W"
			break
		case kInt32:
			makeCmd += "/I"
			break
		case kInt64:
			Abort "No support for 64 bit integers in Igor"
			break
		case kFP32:
			makeCmd += "/S"
			break
		case kFP64:
			makeCmd += "/D"
			break
		default:
			Abort "unknown wave type"
			break
	endswitch
	
	makeCmd += " "
	return makeCmd
End

Function /WAVE FindTypeWildcards(code)
	string code
	
	Make /FREE/N=(0)/T W_TypeWildcards
	
	variable index = 0
	string wildCardName
	for ( ; ; )
		index = strsearch(code, ksTypeWildCard, index)
		if (index == -1)
			break
		endif
		
		wildCardName = ExtractTypeWildcardName(code, index)
		if (strlen(wildCardName) == 0)
			index += 1
			continue
		endif
		
		index += strlen(wildCardName)
		
		FindValue /TEXT=wildCardName /TXOP=4 W_TypeWildcards
		if (V_value == -1)
			Redimension /N=(DimSize(W_TypeWildcards, 0) + 1) W_TypeWildcards
			W_TypeWildcards[DimSize(W_TypeWildcards, 0) - 1] = wildCardName
		endif
	endfor
	
	return W_TypeWildcards
End

Function /S ExtractTypeWildcardName(code, wildcardStartsHere)
	string code
	variable wildcardStartsHere
	
	string wildcardName = ""
	if (strsearch(code, ksTypeWildCard, wildcardStartsHere) != wildcardStartsHere)
		return ""
	endif
	
	variable index = wildcardStartsHere + strlen(ksTypeWildCard)
	// must have number after wildtype string
	if (!IsNumber(code[index]))
		return ""
	endif
	
	string numStr = ""
	do
		numStr += code[index]
		index += 1
	while ((index < strlen(code)) && IsNumber(code[index]))
	
	wildCardName = ksTypeWildCard + numStr
	return wildCardName
End

Function /S TypeIDToTypeName(type)
	variable type
	
	string typeName = ""
	if (type & kUnsigned)
		typeName += "unsigned "
	endif
	type = type & ~kUnsigned
	
	switch (type)
		case kInt8:
			typeName += "8-bit integer"
			break
		case kInt16:
			typeName += "16-bit integer"
			break
		case kInt32:
			typeName += "32-bit integer"
			break
		case kInt64:
			Abort "No support for 64 bit integers in Igor"
			break
		case kFP32:
			typeName += "32-bit floating point"
			break
		case kFP64:
			typeName += "64-bit floating point"
			break
		default:
			Abort "unknown wave type"
			break
	endswitch
	
	return typeName
End

Function FindParenIndex(findStr, startOfSearch)
	string findStr
	variable startOfSearch	// must be one past the opening paren
	
	variable parenCount = 1
	variable length = strlen(findStr)
	variable i
	for (i = startOfSearch; i < length; i+=1)
		if (StringMatch(findStr[i], "("))
			parenCount += 1
			continue
		endif
		if (StringMatch(findStr[i], ")"))
			parenCount -= 1
			if (parenCount == 0)
				return i
			endif
		endif
	endfor
	return -1
End

Function /S IndentIgorCode(code)
	string code
	
	variable curIndent = 0
	variable nLines = ItemsInList(code, "\r")
	string indentedCode = ""
	
	variable i, j
	string thisLine
	for (i = 0; i < nLines; i+=1)
		thisLine = StringFromList(i, code, "\r")
		if (GrepString(thisLine, "(?i)^\\s*(end|endswitch|endif|endfor|while|catch)(\\s|$)"))
			curIndent -= 1
		endif
		for (j = 0; j < curIndent; j+=1)
			indentedCode += "\t"
		endfor
		indentedCode += thisLine + "\r"
		if (GrepString(thisLine, "(?i)^\\s*(function|structure|if|for|do|switch|strswitch|try|menu|static|threadsafe)(\\s|$|\()"))
			curIndent += 1
		endif
	endfor
	
	return indentedCode
End

Static Function Within(a, b, c)
	variable a, b, c
	
	return (limit(a, b, c) == a)
End

Static Function IsNumber(c)
	string c
	
	if (strlen(c) != 1)
		Abort "IsNumber needs single char"
	endif
	
	return Within(char2num(c), 48, 57)
End
