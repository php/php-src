# Microsoft Developer Studio Project File - Name="imap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=imap - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "imap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imap.mak" CFG="imap - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "imap - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\php_build\IMAP\c-client" /I "..\..\TSRM" /D "NDEBUG" /D "IMAP_EXPORTS" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=0 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5nts.lib wsock32.lib winmm.lib cclient.lib /nologo /dll /machine:I386 /out:"Release/php_imap.dll" /libpath:"..\..\Release" /libpath:"..\..\..\php_build\IMAP\C-Client"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\php_build\IMAP\c-client" /I "..\..\TSRM" /D "_DEBUG" /D "IMAP_EXPORTS" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5nts_debug.lib wsock32.lib winmm.lib cclient.lib /nologo /dll /debug /machine:I386 /out:"Debug/php_imap.dll" /pdbtype:sept /libpath:"..\..\Debug" /libpath:"..\..\..\php_build\IMAP\C-Client"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "ZTS" /D "_DEBUG" /D "IMAP_EXPORTS" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib wsock32.lib winmm.lib cclient.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_imap.dll" /pdbtype:sept /libpath:"..\..\..\\" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "imap - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\imap\c-client" /D "ZTS" /D "NDEBUG" /D "IMAP_EXPORTS" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=0 /D HAVE_IMAP2001=1 /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib wsock32.lib winmm.lib cclient.lib Secur32.lib CertIdl.Lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT" /out:"..\..\Release_TS/php_imap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline" /libpath:"..\..\..\php_build\imap"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "imap - Win32 Release"
# Name "imap - Win32 Debug"
# Name "imap - Win32 Debug_TS"
# Name "imap - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\php_imap.c
# End Source File
# Begin Source File

SOURCE=..\..\win32\sendmail.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_imap.h
# End Source File
# Begin Source File

SOURCE=..\..\win32\sendmail.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\IMAP_Win32_HOWTO.txt
# End Source File
# End Target
# End Project
