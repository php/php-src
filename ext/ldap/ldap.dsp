# Microsoft Developer Studio Project File - Name="ldap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ldap - Win32 Debug_TS_SSL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ldap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ldap.mak" CFG="ldap - Win32 Debug_TS_SSL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ldap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Release_TS SASL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Debug_TS SASL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Release_TS_SSL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ldap - Win32 Debug_TS_SSL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ldap - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "COMPILE_DL_LDAP_LDAP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 oldap32.lib olber32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5nts.lib /nologo /dll /machine:I386 /out:"Release/php_ldap.dll" /libpath:"..\..\Release" /libpath:"..\..\..\php_build\openldap\libraries\Release"

!ELSEIF  "$(CFG)" == "ldap - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "COMPILE_DL_LDAP_LDAP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 oldap32.lib olber32.lib php5nts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"Debug/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug" /libpath:"..\..\..\php_build\openldap\libraries\Debug"

!ELSEIF  "$(CFG)" == "ldap - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\openldap\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /D ZEND_DEBUG=1 /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib php5nts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\openldap\libraries\Debug" /libpath:"..\..\Debug"
# ADD LINK32 oldap32.lib olber32.lib php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\openldap\libraries\Debug"

!ELSEIF  "$(CFG)" == "ldap - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\openldap\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /D ZEND_DEBUG=0 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 oldap32.lib olber32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_ldap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\php_build\openldap\libraries\Release" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "ldap - Win32 Release_TS SASL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ldap___Win32_Release_TS_SASL"
# PROP BASE Intermediate_Dir "ldap___Win32_Release_TS_SASL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS_SASL"
# PROP Intermediate_Dir "Release_TS_SASL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_ldap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\php_build\openldap\libraries\Release" /libpath:"..\..\Release_TS_Inline"
# ADD LINK32 oldap32.lib olber32.lib libsasl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_ldap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\php_build\openldap\libraries\Release" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "ldap - Win32 Debug_TS SASL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ldap___Win32_Debug_TS_SASL"
# PROP BASE Intermediate_Dir "ldap___Win32_Debug_TS_SASL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS_SASL"
# PROP Intermediate_Dir "Debug_TS_SASL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"Debug_TS/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\openldap\libraries\Debug"
# ADD LINK32 oldap32.lib olber32.lib libsasl.lib php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\openldap\libraries\Debug"

!ELSEIF  "$(CFG)" == "ldap - Win32 Release_TS_SSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ldap___Win32_Release_TS_SSL"
# PROP BASE Intermediate_Dir "ldap___Win32_Release_TS_SSL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS_SSL"
# PROP Intermediate_Dir "Release_TS_SSL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib php5ts.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_ldap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\php_build\openldap\libraries\Release" /libpath:"..\..\Release_TS_Inline"
# ADD LINK32 php5ts.lib oldap32.lib olber32.lib ssleay32.lib libeay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_ldap.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\php_build\openldap\libraries\Release" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "ldap - Win32 Debug_TS_SSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ldap___Win32_Debug_TS_SSL"
# PROP BASE Intermediate_Dir "ldap___Win32_Debug_TS_SSL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS_SSL"
# PROP Intermediate_Dir "Debug_TS_SSL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "..\..\..\php_build\openldap\include" /D "_DEBUG" /D ZEND_DEBUG=1 /D "ZTS" /D "COMPILE_DL_LDAP" /D "HAVE_LDAP_START_TLS_S" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LDAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_LDAP=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldap32.lib olber32.lib php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\openldap\libraries\Debug"
# ADD LINK32 php5ts_debug.lib oldap32.lib olber32.lib ssleay32.lib libeay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_ldap.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\openldap\libraries\Debug"

!ENDIF 

# Begin Target

# Name "ldap - Win32 Release"
# Name "ldap - Win32 Debug"
# Name "ldap - Win32 Debug_TS"
# Name "ldap - Win32 Release_TS"
# Name "ldap - Win32 Release_TS SASL"
# Name "ldap - Win32 Debug_TS SASL"
# Name "ldap - Win32 Release_TS_SSL"
# Name "ldap - Win32 Debug_TS_SSL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ldap.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_ldap.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\LDAP_Win32_HOWTO.txt
# End Source File
# End Target
# End Project
