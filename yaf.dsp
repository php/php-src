# Microsoft Developer Studio Project File - Name="yaf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=yaf - Win32 Release_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "yaf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "yaf.mak" CFG="yaf - Win32 Release_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "yaf - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "yaf - Win32 Release_NTS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "yaf - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "yaf - Win32 Debug_NTS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "yaf - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_YAF" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAF_EXPORTS" /D "COMPILE_DL_YAF" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_YAF=1 /D "LIBZEND_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_yaf.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "yaf - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_YAF" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAF_EXPORTS" /D "COMPILE_DL_YAF" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_YAF=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib /nologo /dll /machine:I386 /out:"..\..\Debug_TS/php_yaf.dll" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "yaf - Win32 Release_NTS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_NTS"
# PROP BASE Intermediate_Dir "Release_NTS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_NTS"
# PROP Intermediate_Dir "Release_NTS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_YAF" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAF_EXPORTS" /D "COMPILE_DL_YAF" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_YAF=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5.lib /nologo /dll /machine:I386 /out:"..\..\Release_NTS/php_yaf.dll" /libpath:"..\..\Release_NTS"

!ELSEIF  "$(CFG)" == "yaf - Win32 Debug_NTS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_NTS"
# PROP BASE Intermediate_Dir "Debug_NTS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_NTS"
# PROP Intermediate_Dir "Debug_NTS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_YAF" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAF_EXPORTS" /D "COMPILE_DL_YAF" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_YAF=1 /D "LIBZEND_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5_debug.lib /nologo /dll /machine:I386 /out:"..\..\Debug_NTS/php_yaf.dll" /libpath:"..\..\Debug_NTS"

!ENDIF 

# Begin Target

# Name "yaf - Win32 Release_TS"
# Name "yaf - Win32 Debug_TS"
# Name "yaf - Win32 Release_NTS"
# Name "yaf - Win32 Debug_NTS"
# Begin Group "Yaf Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\yaf.c
# End Source File
# Begin Source File

SOURCE=.\yaf_application.c
# End Source File
# Begin Source File

SOURCE=.\yaf_loader.c
# End Source File
# Begin Source File

SOURCE=.\yaf_dispatcher.c
# End Source File
# Begin Source File

SOURCE=.\yaf_bootstrap.c
# End Source File
# Begin Source File

SOURCE=.\yaf_config.c
# End Source File
# Begin Source File

SOURCE=.\yaf_registry.c
# End Source File
# Begin Source File

SOURCE=.\yaf_controller.c
# End Source File
# Begin Source File

SOURCE=.\yaf_action.c
# End Source File
# Begin Source File

SOURCE=.\yaf_view.c
# End Source File
# Begin Source File

SOURCE=.\yaf_request.c
# End Source File
# Begin Source File

SOURCE=.\yaf_response.c
# End Source File
# Begin Source File

SOURCE=.\yaf_router.c
# End Source File
# Begin Source File

SOURCE=.\yaf_exception.c
# End Source File
# Begin Source File

SOURCE=.\yaf_plugin.c
# End Source File
# Begin Source File

SOURCE=.\yaf_session.c
# End Source File

# End Group
# Begin Group "Yaf Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_yaf.h
# End Source File
# Begin Source File

SOURCE=.\yaf_application.h
# End Source File
# Begin Source File

SOURCE=.\yaf_bootstrap.h
# End Source File
# Begin Source File

SOURCE=.\yaf_loader.h
# End Source File
# Begin Source File

SOURCE=.\yaf_config.h
# End Source File
# Begin Source File

SOURCE=.\yaf_registry.h
# End Source File
# Begin Source File

SOURCE=.\yaf_controller.h
# End Source File
# Begin Source File

SOURCE=.\yaf_view.h
# End Source File
# Begin Source File

SOURCE=.\php_request.h
# End Source File
# Begin Source File

SOURCE=.\yaf_router.h
# End Source File
# Begin Source File

SOURCE=.\yaf_exception.h
# End Source File
# Begin Source File

SOURCE=.\yaf_plugin.h
# End Source File
# Begin Source File

SOURCE=.\yaf_session.h
# End Source File
# End Group
# End Target
# End Project
