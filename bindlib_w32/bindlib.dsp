# Microsoft Developer Studio Project File - Name="bindlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bindlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bindlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bindlib.mak" CFG="bindlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bindlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "bindlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bindlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WINNT" /YX /FD /c
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\resolv.lib"

!ELSEIF  "$(CFG)" == "bindlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WINNT" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\resolv.lib"

!ENDIF 

# Begin Target

# Name "bindlib - Win32 Release"
# Name "bindlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\base64.c
# End Source File
# Begin Source File

SOURCE=.\gethnamaddr.c
# End Source File
# Begin Source File

SOURCE=.\getnetbyaddr.c
# End Source File
# Begin Source File

SOURCE=.\getnetbyname.c
# End Source File
# Begin Source File

SOURCE=.\getnetent.c
# End Source File
# Begin Source File

SOURCE=.\getnetnamadr.c
# End Source File
# Begin Source File

SOURCE=.\herror.c
# End Source File
# Begin Source File

SOURCE=.\hostnamelen.c
# End Source File
# Begin Source File

SOURCE=.\inet_addr.c
# End Source File
# Begin Source File

SOURCE=.\inet_net_ntop.c
# End Source File
# Begin Source File

SOURCE=.\inet_net_pton.c
# End Source File
# Begin Source File

SOURCE=.\inet_neta.c
# End Source File
# Begin Source File

SOURCE=.\inet_ntop.c
# End Source File
# Begin Source File

SOURCE=.\inet_pton.c
# End Source File
# Begin Source File

SOURCE=.\nsap_addr.c
# End Source File
# Begin Source File

SOURCE=.\res_comp.c
# End Source File
# Begin Source File

SOURCE=.\res_data.c
# End Source File
# Begin Source File

SOURCE=.\res_debug.c
# End Source File
# Begin Source File

SOURCE=.\res_init.c
# End Source File
# Begin Source File

SOURCE=.\res_mkquery.c
# End Source File
# Begin Source File

SOURCE=.\res_nt_misc.c
# End Source File
# Begin Source File

SOURCE=.\res_query.c
# End Source File
# Begin Source File

SOURCE=.\res_send.c
# End Source File
# Begin Source File

SOURCE=.\sethostent.c
# End Source File
# Begin Source File

SOURCE=.\writev.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resolv\netdb.h
# End Source File
# Begin Source File

SOURCE=.\resolv\resolv.h
# End Source File
# End Group
# End Target
# End Project
