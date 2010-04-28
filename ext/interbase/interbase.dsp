# Microsoft Developer Studio Project File - Name="interbase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=interbase - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "interbase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "interbase.mak" CFG="interbase - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "interbase - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "interbase - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "interbase"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "interbase - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\..\php_build\Interbase SDK\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTERBASE_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=1 /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\..\php_build\Interbase SDK\include" /I "..\..\..\bindlib_w32" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTERBASE_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_IBASE=1 /D "COMPILE_DL_INTERBASE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ib_util_ms.lib gds32_ms.lib php5ts_debug.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib gds32_ms.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_interbase.dll" /pdbtype:sept /libpath:"..\..\..\php_build\Interbase SDK\lib_ms" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "interbase - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\..\php_build\Interbase SDK\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTERBASE_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_IBASE=1 /D ZEND_DEBUG=0 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\main" /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\..\php_build\Interbase SDK\include" /I "..\..\..\bindlib_w32" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTERBASE_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_IBASE=1 /D "COMPILE_DL_INTERBASE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php5ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ib_util_ms.lib gds32_ms.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gds32_ms.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_interbase.dll" /libpath:"..\..\..\php_build\Interbase SDK\lib_ms" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ENDIF 

# Begin Target

# Name "interbase - Win32 Debug_TS"
# Name "interbase - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ibase_blobs.c
# End Source File
# Begin Source File

SOURCE=.\ibase_events.c
# End Source File
# Begin Source File

SOURCE=.\ibase_query.c
# End Source File
# Begin Source File

SOURCE=.\ibase_service.c
# End Source File
# Begin Source File

SOURCE=.\interbase.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_interbase.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\interbase.rc
# ADD BASE RSC /l 0x413
# ADD RSC /l 0x413 /i "..\..\main" /i "..\..\win32" /d "PHP_H"
# End Source File
# End Group
# End Target
# End Project
