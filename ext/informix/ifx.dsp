# Microsoft Developer Studio Project File - Name="ifx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ifx - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ifx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ifx.mak" CFG="ifx - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ifx - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ifx - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ifx - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "c:\ifx-client\incl\esql" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /D HAVE_IFX=1 /D HAVE_IUS=1 /D "ZTS" /D ZEND_WIN32=1 /D PHP_WIN32=1 /D IFX_VERSION=921 /D COMPILE_DL_INFORMIX=1 /D ZEND_DEBUG=0 /D HAVE_IFX_IUS=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 isqlt09a.lib php4ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\Release_TS/php_ifx.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline" /libpath:"c:\ifx-client\lib"

!ELSEIF  "$(CFG)" == "ifx - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ifx___W0"
# PROP BASE Intermediate_Dir "ifx___W0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "c:\ifx-client\incl\esql" /D "_DEBUG" /D ZEND_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL" /D HAVE_IFX=1 /D HAVE_IUS=1 /D "ZTS" /D ZEND_WIN32=1 /D PHP_WIN32=1 /D IFX_VERSION=921 /D COMPILE_DL_INFORMIX=1 /D HAVE_IFX_IUS=1 /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 isqlt09a.lib php4ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug_TS/php_ifx.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"c:\ifx-client\lib"

!ENDIF 

# Begin Target

# Name "ifx - Win32 Release_TS"
# Name "ifx - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ifx.c
# End Source File
# Begin Source File

SOURCE=.\ifx.ec

!IF  "$(CFG)" == "ifx - Win32 Release_TS"

# Begin Custom Build
InputPath=.\ifx.ec

".\ifx.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	esql.exe -dcmdl -p -G -subsystem:windows -EDHAVE_IFX_IUS=1 -thread -I..\.. ifx.ec

# End Custom Build

!ELSEIF  "$(CFG)" == "ifx - Win32 Debug_TS"

# Begin Custom Build
InputPath=.\ifx.ec

".\ifx.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	esql.exe -dcmdl -p -G -subsystem:windows -EDHAVE_IFX_IUS=1 -thread -I..\.. ifx.ec

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_informix.h
# End Source File
# Begin Source File

SOURCE=.\php_informix_includes.h
# End Source File
# End Group
# End Target
# End Project
