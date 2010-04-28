# Microsoft Developer Studio Project File - Name="php5ts_cli" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=php5ts_cli - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php5ts_cli.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php5ts_cli.mak" CFG="php5ts_cli - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php5ts_cli - Win32 Release_TS" (based on "Win32 (x86) Console Application")
!MESSAGE "php5ts_cli - Win32 Debug_TS" (based on "Win32 (x86) Console Application")
!MESSAGE "php5ts_cli - Win32 Release_TS_inline" (based on "Win32 (x86) Console Application")
!MESSAGE "php5ts_cli - Win32 Release_TSDbg" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php5ts_cli - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_CONSOLE" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 php5ts.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\Release_TS\cli\php.exe" /libpath:"..\Release_TS"

!ELSEIF  "$(CFG)" == "php5ts_cli - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "DEBUG" /D "_DEBUG" /D ZEND_DEBUG=1 /D "_CONSOLE" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib netapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib /nologo /version:4.0 /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmt" /out:"..\Debug_TS\cli\php.exe" /pdbtype:sept /libpath:"..\Debug_TS"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "php5ts_cli - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php5ts___Win32_Release_TS_inline"
# PROP BASE Intermediate_Dir "php5ts___Win32_Release_TS_inline"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "regex" /I "..\bindlib_w32" /I "Zend" /I "tsrm" /D "NDEBUG" /D "MSVC5" /D "_CONSOLE" /D "ZTS" /D "WIN32" /D "_MBCS" /D ZEND_DEBUG=0 /Fr /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZEND_WIN32_FORCE_INLINE" /D "_CONSOLE" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php5ts.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"Release_TS\php.exe" /libpath:"Release_TS"
# ADD LINK32 php5ts.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\Release_TS_inline\cli\php.exe" /libpath:"..\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5ts_cli - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php5ts___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "php5ts___Win32_Release_TSDbg"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TSDbg"
# PROP Intermediate_Dir "Release_TSDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_CONSOLE" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /Fr /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_CONSOLE" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php5ts.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"..\Release_TS\php.exe" /libpath:"..\Release_TS"
# ADD LINK32 php5ts.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:4.0 /subsystem:console /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\Release_TSDbg\php.exe" /libpath:"..\Release_TSDbg"

!ENDIF 

# Begin Target

# Name "php5ts_cli - Win32 Release_TS"
# Name "php5ts_cli - Win32 Debug_TS"
# Name "php5ts_cli - Win32 Release_TS_inline"
# Name "php5ts_cli - Win32 Release_TSDbg"
# Begin Group "Source Files"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\sapi\cli\getopt.c
# End Source File
# Begin Source File

SOURCE=..\sapi\cli\php_cli.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\sapi\cli\php_getopt.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\php5ts_cli.rc
# End Source File
# Begin Source File

SOURCE=.\php5ts_cli.rc2
# End Source File
# End Target
# End Project
