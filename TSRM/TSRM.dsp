# Microsoft Developer Studio Project File - Name="TSRM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TSRM - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TSRM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TSRM.mak" CFG="TSRM - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TSRM - Win32 Debug_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "TSRM - Win32 Release_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "TSRM - Win32 Release_TS_inline" (based on "Win32 (x86) Static Library")
!MESSAGE "TSRM - Win32 Release_TSDbg" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TSRM - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TSRM___Win32_Debug_TS"
# PROP BASE Intermediate_Dir "TSRM___Win32_Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Projects\TSRM" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "ZTS" /D "_LIB" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D TSRM_DEBUG=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TSRM - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TSRM___Win32_Release_TS"
# PROP BASE Intermediate_Dir "TSRM___Win32_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "ZTS" /D "_LIB" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D TSRM_DEBUG=0 /YX /FD /c
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TSRM - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TSRM___Win32_Release_TS_inline"
# PROP BASE Intermediate_Dir "TSRM___Win32_Release_TS_inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "TSRM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "ZTS" /D "_LIB" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D TSRM_DEBUG=0 /YX /FD /c
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TSRM - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TSRM___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "TSRM___Win32_Release_TSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TSDbg"
# PROP Intermediate_Dir "Release_TSDbg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "ZTS" /D "_LIB" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D TSRM_DEBUG=0 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I "." /D "NDEBUG" /D "ZTS" /D "_LIB" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D TSRM_DEBUG=0 /YX /FD /c
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "TSRM - Win32 Debug_TS"
# Name "TSRM - Win32 Release_TS"
# Name "TSRM - Win32 Release_TS_inline"
# Name "TSRM - Win32 Release_TSDbg"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\TSRM.c
# End Source File
# Begin Source File

SOURCE=.\tsrm_strtok_r.c
# End Source File
# Begin Source File

SOURCE=.\tsrm_virtual_cwd.c
# End Source File
# Begin Source File

SOURCE=.\tsrm_win32.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\readdir.h
# End Source File
# Begin Source File

SOURCE=.\TSRM.h
# End Source File
# Begin Source File

SOURCE=.\tsrm_config.w32.h
# End Source File
# Begin Source File

SOURCE=.\tsrm_config_common.h
# End Source File
# Begin Source File

SOURCE=.\tsrm_strtok_r.h
# End Source File
# Begin Source File

SOURCE=.\tsrm_virtual_cwd.h
# End Source File
# Begin Source File

SOURCE=.\tsrm_win32.h
# End Source File
# End Group
# End Target
# End Project
