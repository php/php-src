# Microsoft Developer Studio Project File - Name="w32api" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=w32api - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "w32api.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "w32api.mak" CFG="w32api - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "w32api - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "w32api - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "w32api - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "w32api___Win32_Debug_TS"
# PROP BASE Intermediate_Dir "w32api___Win32_Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "W32API_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "W32API_EXPORTS" /D HAVE_W32API=1 /D "COMPILE_DL_W32API" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../Debug_TS/php_w32api.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "w32api - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "w32api___Win32_Release_TS"
# PROP BASE Intermediate_Dir "w32api___Win32_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "W32API_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "W32API_EXPORTS" /D "HAVE_W32API" /D "COMPILE_DL_W32API" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=0 /YX /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /debugtype:both /machine:I386 /out:"../../Release_TS/php_w32api.dll" /libpath:"..\..\Release_TS"
# SUBTRACT LINK32 /incremental:yes

!ENDIF 

# Begin Target

# Name "w32api - Win32 Debug_TS"
# Name "w32api - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\w32api.c
# End Source File
# Begin Source File

SOURCE=.\w32api_function_definition_parser.c
# End Source File
# Begin Source File

SOURCE=.\w32api_function_definition_scanner.c
# End Source File
# Begin Source File

SOURCE=.\w32api_type_definition_parser.c
# End Source File
# Begin Source File

SOURCE=.\w32api_type_definition_scanner.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_w32api.h
# End Source File
# Begin Source File

SOURCE=.\w32api_function_definition_parser.h
# End Source File
# Begin Source File

SOURCE=.\w32api_type_definition_parser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Parsers"

# PROP Default_Filter ".y"
# Begin Source File

SOURCE=.\w32api_function_definition_parser.y

!IF  "$(CFG)" == "w32api - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_function_definition_parser.y

BuildCmds= \
	bison --output=w32api_function_definition_parser.c -v -d -p w32api_function_definition_ w32api_function_definition_parser.y

"$(InputDir)/w32api_function_definition_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)/w32api_function_definition_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "w32api - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_function_definition_parser.y

BuildCmds= \
	bison --output=w32api_function_definition_parser.c -v -d -p w32api_function_definition_ w32api_function_definition_parser.y

"$(InputDir)/w32api_function_definition_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)/w32api_function_definition_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\w32api_type_definition_parser.y

!IF  "$(CFG)" == "w32api - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_type_definition_parser.y

BuildCmds= \
	bison --output=w32api_type_definition_parser.c -v -d -p w32api_type_definition_ w32api_type_definition_parser.y

"$(InputDir)/w32api_type_definition_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)/w32api_type_definition_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "w32api - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_type_definition_parser.y

BuildCmds= \
	bison --output=w32api_type_definition_parser.c -v -d -p w32api_type_definition_ w32api_type_definition_parser.y

"$(InputDir)/w32api_type_definition_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)/w32api_type_definition_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Scanners"

# PROP Default_Filter ".l"
# Begin Source File

SOURCE=.\w32api_function_definition_scanner.l

!IF  "$(CFG)" == "w32api - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_function_definition_scanner.l

"$(InputDir)\w32api_function_definition_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -B -i -Pw32api_function_definition -ow32api_function_definition_scanner.c w32api_function_definition_scanner.l

# End Custom Build

!ELSEIF  "$(CFG)" == "w32api - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_function_definition_scanner.l

"$(InputDir)\w32api_function_definition_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -B -i -Pw32api_function_definition -ow32api_function_definition_scanner.c w32api_function_definition_scanner.l

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\w32api_type_definition_scanner.l

!IF  "$(CFG)" == "w32api - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_type_definition_scanner.l

"$(InputDir)/w32api_type_definition_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -B -i -Pw32api_type_definition -ow32api_type_definition_scanner.c w32api_type_definition_scanner.l

# End Custom Build

!ELSEIF  "$(CFG)" == "w32api - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=.\w32api_type_definition_scanner.l

"$(InputDir)/w32api_type_definition_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -B -i -Pw32api_type_definition -ow32api_type_definition_scanner.c w32api_type_definition_scanner.l

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
