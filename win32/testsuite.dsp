# Microsoft Developer Studio Project File - Name="testsuite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=testsuite - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "testsuite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "testsuite.mak" CFG="testsuite - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testsuite - Win32 Release_TS_Inline" (based on "Win32 (x86) Generic Project")
!MESSAGE "testsuite - Win32 Release_TS" (based on "Win32 (x86) Generic Project")
!MESSAGE "testsuite - Win32 Release_TSDbg" (based on "Win32 (x86) Generic Project")
!MESSAGE "testsuite - Win32 Debug_TS" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "testsuite - Win32 Release_TS_Inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS_Inline"
# PROP BASE Intermediate_Dir "Release_TS_Inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS_Inline"
# PROP Intermediate_Dir "..\..\"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "testsuite - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS"
# PROP Intermediate_Dir "..\..\"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "testsuite - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TSDbg"
# PROP BASE Intermediate_Dir "Release_TSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TSDbg"
# PROP Intermediate_Dir "..\..\"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "testsuite - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug_TS"
# PROP Intermediate_Dir "..\..\"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "testsuite - Win32 Release_TS_Inline"
# Name "testsuite - Win32 Release_TS"
# Name "testsuite - Win32 Release_TSDbg"
# Name "testsuite - Win32 Debug_TS"
# Begin Source File

SOURCE=..\results.txt

!IF  "$(CFG)" == "testsuite - Win32 Release_TS_Inline"

# PROP Intermediate_Dir "..\Release_TS_Inline"
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Running Testsuite, please wait...
IntDir=.\..\Release_TS_Inline
InputPath=..\results.txt

"..\..\results.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set TOP_BUILDDIR=Release_TS_inline 
	set TEST_DIR=tests 
	$(IntDir)\php.exe -q ..\run-tests.php > ..\results.txt 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "testsuite - Win32 Release_TS"

# PROP Intermediate_Dir "..\Release_TS"
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Running Testsuite, please wait...
IntDir=.\..\Release_TS
InputPath=..\results.txt

"..\..\results.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set TOP_BUILDDIR=Release_TS 
	set TEST_DIR=tests 
	$(IntDir)\php.exe -q ..\run-tests.php > ..\results.txt 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "testsuite - Win32 Release_TSDbg"

# PROP Intermediate_Dir "..\Release_TSDbg"
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Running Testsuite, please wait...
IntDir=.\..\Release_TSDbg
InputPath=..\results.txt

"..\..\results.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set TOP_BUILDDIR=Release_TSDbg 
	set TEST_DIR=tests 
	$(IntDir)\php.exe -q ..\run-tests.php > ..\results.txt 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "testsuite - Win32 Debug_TS"

# PROP Intermediate_Dir "..\Debug_TS"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
