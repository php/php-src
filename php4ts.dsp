# Microsoft Developer Studio Project File - Name="php4ts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=php4ts - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php4ts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php4ts.mak" CFG="php4ts - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php4ts - Win32 Release_TS" (based on "Win32 (x86) Console Application")
!MESSAGE "php4ts - Win32 Debug_TS" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php4ts - Win32 Release_TS"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\libzend" /I "." /I "regex\\" /I "d:\src\bind\include" /I "..\tsrm" /D "NDEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "COMPILE_LIBZEND" /D "ZTS" /D "TSRM_EXPORTS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libzendts.lib tsrm.lib /nologo /version:3.0 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"Release_TS\php.exe" /libpath:"lib" /libpath:"..\tsrm\release_TS" /libpath:"..\libzend\Release_TS" /libpath:"."

!ELSEIF  "$(CFG)" == "php4ts - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "include" /I "..\libzend" /I "." /I "regex\\" /I "d:\src\bind\include" /I "..\tsrm" /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "COMPILE_LIBZEND" /D "ZTS" /D "TSRM_EXPORTS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TSRM.lib libzendts.lib php4libts.lib lib44bsd95.lib resolv.lib /nologo /version:3.0 /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmt" /out:"Debug_TS\php.exe" /pdbtype:sept /libpath:"lib" /libpath:"..\TSRM\debug_ts" /libpath:"..\libzend\Debug_TS" /libpath:"Debug_TS"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "php4ts - Win32 Release_TS"
# Name "php4ts - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\cgi_main.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# End Group
# End Target
# End Project
