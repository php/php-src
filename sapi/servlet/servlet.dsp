# Microsoft Developer Studio Project File - Name="servlet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=servlet - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "servlet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "servlet.mak" CFG="servlet - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "servlet - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "servlet - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "servlet - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "servlet - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "servlet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\Release"
# PROP BASE Intermediate_Dir "..\..\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "NDEBUG" /D ZEND_DEBUG=0 /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /dll /machine:I386 /out:"..\..\Release/phpsrvlt.dll" /libpath:"$(JAVA_HOME)\lib" /libpath:"..\..\Release"

!ELSEIF  "$(CFG)" == "servlet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\Debug"
# PROP BASE Intermediate_Dir "..\..\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "_DEBUG" /D ZEND_DEBUG=1 /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /dll /debug /machine:I386 /out:"..\..\Debug/phpsrvlt.dll" /pdbtype:sept /libpath:"$(JAVA_HOME)\lib" /libpath:"..\..\Debug"

!ELSEIF  "$(CFG)" == "servlet - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\Debug_TS"
# PROP BASE Intermediate_Dir "..\..\Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "_DEBUG" /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\TSRM" /I "..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "_DEBUG" /D ZEND_DEBUG=1 /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /D "ZTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /dll /debug /machine:I386 /out:"..\..\Debug_TS/phpsrvlt.dll" /pdbtype:sept /libpath:"$(JAVA_HOME)\lib" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "servlet - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\Release_TS"
# PROP BASE Intermediate_Dir "..\..\Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "NDEBUG" /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\TSRM" /I "..\..\Zend" /I "..\..\main" /I "$(JAVA_HOME)\include\win32" /I "$(JAVA_HOME)\include" /I "..\..\..\bindlib_w32" /D "NDEBUG" /D ZEND_DEBUG=0 /D "PHP_WIN32" /D "ZEND_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /D "ZTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /dll /machine:I386 /out:"..\..\Release_TS/phpsrvlt.dll" /libpath:"$(JAVA_HOME)\lib" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ENDIF 

# Begin Target

# Name "servlet - Win32 Release"
# Name "servlet - Win32 Debug"
# Name "servlet - Win32 Debug_TS"
# Name "servlet - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ext\rpc\java\java.c
# End Source File
# Begin Source File

SOURCE=.\servlet.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Java Files"

# PROP Default_Filter "java"
# Begin Source File

SOURCE=.\servlet.java

!IF  "$(CFG)" == "servlet - Win32 Release"

USERDEP__SERVL="..\..\ext\rpc\java\reflect.java"	
# Begin Custom Build
OutDir=.\..\..\Release
InputPath=.\servlet.java

"$(OutDir)\phpsrvlt.jar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist net mkdir net 
	if not exist net\php mkdir net\php 
	copy *.java net\php > nul 
	copy ..\..\ext\rpc\java\reflect.java net\php > nul 
	echo library=phpsrvlt>net/php/reflect.properties 
	echo library=phpsrvlt>net/php/servlet.properties 
	cd net\php 
	$(JAVA_HOME)\bin\javac -O *.java 
	cd ..\.. 
	$(JAVA_HOME)\bin\jar c0f $(OutDir)\phpsrvlt.jar net\php\*.class net\php\*.properties 
	erase net\php\servlet.* 
	erase net\php\formatter.* 
	erase net\php\reflect.* 
	rmdir net\php 
	rmdir net 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "servlet - Win32 Debug"

USERDEP__SERVL="..\..\ext\rpc\java\reflect.java"	
# Begin Custom Build
OutDir=.\..\..\Debug
InputPath=.\servlet.java

"$(OutDir)\phpsrvlt.jar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist net mkdir net 
	if not exist net\php mkdir net\php 
	copy *.java net\php > nul 
	copy ..\..\ext\rpc\java\reflect.java net\php > nul 
	echo library=phpsrvlt>net/php/reflect.properties 
	echo library=phpsrvlt>net/php/servlet.properties 
	cd net\php 
	$(JAVA_HOME)\bin\javac -g *.java 
	cd ..\.. 
	$(JAVA_HOME)\bin\jar c0f $(OutDir)\phpsrvlt.jar net\php\*.class net\php\*.properties 
	erase net\php\servlet.* 
	erase net\php\formatter.* 
	erase net\php\reflect.* 
	rmdir net\php 
	rmdir net 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "servlet - Win32 Debug_TS"

USERDEP__SERVL="..\..\ext\rpc\java\reflect.java"	
# Begin Custom Build
OutDir=.\..\..\Debug_TS
InputPath=.\servlet.java

"$(OutDir)\phpsrvlt.jar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist net mkdir net 
	if not exist net\php mkdir net\php 
	copy *.java net\php > nul 
	copy ..\..\ext\rpc\java\reflect.java net\php > nul 
	echo library=phpsrvlt>net/php/reflect.properties 
	echo library=phpsrvlt>net/php/servlet.properties 
	cd net\php 
	$(JAVA_HOME)\bin\javac -g *.java 
	cd ..\.. 
	$(JAVA_HOME)\bin\jar c0f $(OutDir)\phpsrvlt.jar net\php\*.class net\php\*.properties 
	erase net\php\servlet.* 
	erase net\php\formatter.* 
	erase net\php\reflect.* 
	rmdir net\php 
	rmdir net 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "servlet - Win32 Release_TS"

USERDEP__SERVL="..\..\ext\rpc\java\reflect.java"	
# Begin Custom Build
OutDir=.\..\..\Release_TS
InputPath=.\servlet.java

"$(OutDir)\phpsrvlt.jar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist net mkdir net 
	if not exist net\php mkdir net\php 
	copy *.java net\php > nul 
	copy ..\..\ext\rpc\java\reflect.java net\php > nul 
	echo library=phpsrvlt>net/php/reflect.properties 
	echo library=phpsrvlt>net/php/servlet.properties 
	cd net\php 
	$(JAVA_HOME)\bin\javac -O *.java 
	cd ..\.. 
	$(JAVA_HOME)\bin\jar c0f $(OutDir)\phpsrvlt.jar net\php\*.class net\php\*.properties 
	erase net\php\servlet.* 
	erase net\php\formatter.* 
	erase net\php\reflect.* 
	rmdir net\php 
	rmdir net 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\servlet.java
# End Source File
# End Group
# Begin Source File

SOURCE=.\jtest.php
# End Source File
# End Target
# End Project
