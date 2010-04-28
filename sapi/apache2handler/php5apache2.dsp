# Microsoft Developer Studio Project File - Name="php5apache2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php5apache2 - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php5apache2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php5apache2.mak" CFG="php5apache2 - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php5apache2 - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php5apache2 - Win32 Release_TS_inline" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php5apache2 - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php5apache2 - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_APACHE2_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "...\..\include" /I "..\..\win32" /I "..\..\Zend" /I "..\.." /I "..\..\..\bindlib_w32" /I "..\..\main" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "NDEBUG" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /D "_USRDLL" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib libhttpd.lib libapr.lib libaprutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"..\..\Release_TS" /libpath:"..\..\TSRM\Release_TS" /libpath:"..\..\Zend\Release_TS"

!ELSEIF  "$(CFG)" == "php5apache2 - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS_inline"
# PROP BASE Intermediate_Dir "Release_TS_inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_APACHE2_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "...\..\include" /I "..\..\win32" /I "..\..\Zend" /I "..\.." /I "..\..\..\bindlib_w32" /I "..\..\main" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "ZEND_WIN32_FORCE_INLINE" /D "NDEBUG" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /D "_USRDLL" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib libhttpd.lib libapr.lib libaprutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS_inline/php5apache2.dll" /libpath:"..\..\Release_TS_inline" /libpath:"..\..\TSRM\Release_TS_inline" /libpath:"..\..\Zend\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5apache2 - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_APACHE2_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "...\..\include" /I "..\..\win32" /I "..\..\Zend" /I "..\.." /I "..\..\..\bindlib_w32" /I "..\..\main" /I "..\..\TSRM" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /D "_USRDLL" /D "WIN32" /D "_MBCS" /YX /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php5ts_debug.lib libhttpd.lib libapr.lib libaprutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\TSRM\Debug_TS" /libpath:"..\..\Zend\Debug_TS"

!ENDIF 

# Begin Target

# Name "php5apache2 - Win32 Release_TS"
# Name "php5apache2 - Win32 Release_TS_inline"
# Name "php5apache2 - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\apache_config.c
# End Source File
# Begin Source File

SOURCE=.\mod_php5.c
# End Source File
# Begin Source File

SOURCE=.\php_functions.c
# End Source File
# Begin Source File

SOURCE=.\sapi_apache2.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_apache.h
# End Source File
# End Group
# End Target
# End Project
