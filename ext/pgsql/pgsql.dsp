# Microsoft Developer Studio Project File - Name="pgsql" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pgsql - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pgsql.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgsql.mak" CFG="pgsql - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pgsql - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgsql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgsql - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgsql - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgsql - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGSQL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\php_build\include\pgsql" /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D "HAVE_PQCMDTUPLES" /D "HAVE_PQCLIENTENCODING" /D "HAVE_PQESCAPE" /D "HAVE_PG_CONFIG_H" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libpqdll.lib php5ts.lib /nologo /dll /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../../Release/pgsql.dll" /libpath:"..\..\..\PostgreSQL\lib" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "pgsql - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGSQL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\php_build\include\pgsql" /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D "_DEBUG" /D ZEND_DEBUG=1 /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D "HAVE_PQCMDTUPLES" /D "HAVE_PQCLIENTENCODING" /D "HAVE_PQESCAPE" /D "HAVE_PG_CONFIG_H" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"../../Debug/pgsql.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "pgsql - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\..\PostgreSQL\include" /I "..\..\..\bindlib_w32" /D "NDEBUG" /D "ZTS" /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D ZTS=1 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\php_build\include\pgsql" /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D "NDEBUG" /D "ZTS" /D ZEND_DEBUG=0 /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D "HAVE_PQCMDTUPLES" /D "HAVE_PQCLIENTENCODING" /D "HAVE_PQESCAPE" /D "HAVE_PG_CONFIG_H" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libpqdll.lib php5ts.lib /nologo /dll /machine:I386 /libpath:"..\..\..\PostgreSQL\lib" /libpath:"..\..\Release_TS"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libpq.lib php5ts.lib /nologo /dll /machine:I386 /out:"../../Release_TS/php_pgsql.dll" /libpath:"..\..\..\php_build\postgresql\src\interfaces\libpq\Release" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "pgsql - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D ZTS=1 /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\php_build\include\pgsql" /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D "_DEBUG" /D "ZTS" /D ZEND_DEBUG=1 /D "PGSQL_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "COMPILE_DL_PGSQL" /D HAVE_PGSQL=1 /D "HAVE_PQCMDTUPLES" /D "HAVE_PQCLIENTENCODING" /D "HAVE_PQESCAPE" /D "HAVE_PG_CONFIG_H" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpq.lib /nologo /dll /debug /machine:I386 /out:"../../Debug_TS/pgsql.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "pgsql - Win32 Release"
# Name "pgsql - Win32 Debug"
# Name "pgsql - Win32 Release_TS"
# Name "pgsql - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pgsql.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_pgsql.h
# End Source File
# End Group
# End Target
# End Project
