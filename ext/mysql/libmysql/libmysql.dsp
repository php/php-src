# Microsoft Developer Studio Project File - Name="libmysql" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmysql - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmysql.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmysql.mak" CFG="libmysql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmysql - Win32 Debug_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Release_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Release_TS_inline" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Release_inline" (based on "Win32 (x86) Static Library")
!MESSAGE "libmysql - Win32 Release_TSDbg" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmysql - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS_inline"
# PROP BASE Intermediate_Dir "Release_TS_inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libmysql___Win32_Debug"
# PROP BASE Intermediate_Dir "libmysql___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmysql___Win32_Release"
# PROP BASE Intermediate_Dir "libmysql___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Release_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmysql___Win32_Release_inline"
# PROP BASE Intermediate_Dir "libmysql___Win32_Release_inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_inline"
# PROP Intermediate_Dir "Release_inline"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmysql - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmysql___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "libmysql___Win32_Release_TSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TSDbg"
# PROP Intermediate_Dir "Release_TSDbg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I "." /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "USE_TLS" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libmysql - Win32 Debug_TS"
# Name "libmysql - Win32 Release_TS"
# Name "libmysql - Win32 Release_TS_inline"
# Name "libmysql - Win32 Debug"
# Name "libmysql - Win32 Release"
# Name "libmysql - Win32 Release_inline"
# Name "libmysql - Win32 Release_TSDbg"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\array.c
# End Source File
# Begin Source File

SOURCE=.\bchange.c
# End Source File
# Begin Source File

SOURCE=.\bmove.c
# End Source File
# Begin Source File

SOURCE=.\bmove_upp.c
# End Source File
# Begin Source File

SOURCE=.\charset.c
# End Source File
# Begin Source File

SOURCE=.\ctype.c
# End Source File
# Begin Source File

SOURCE=.\dbug.c
# End Source File
# Begin Source File

SOURCE=.\default.c
# End Source File
# Begin Source File

SOURCE=.\dll.c
# End Source File
# Begin Source File

SOURCE=.\errmsg.c
# End Source File
# Begin Source File

SOURCE=.\errors.c
# End Source File
# Begin Source File

SOURCE=.\get_password.c
# End Source File
# Begin Source File

SOURCE=.\int2str.c
# End Source File
# Begin Source File

SOURCE=.\is_prefix.c
# End Source File
# Begin Source File

SOURCE=.\libmysql.c
# End Source File
# Begin Source File

SOURCE=.\list.c
# End Source File
# Begin Source File

SOURCE=.\longlong2str.c
# End Source File
# Begin Source File

SOURCE=.\mf_casecnv.c
# End Source File
# Begin Source File

SOURCE=.\mf_dirname.c
# End Source File
# Begin Source File

SOURCE=.\mf_fn_ext.c
# End Source File
# Begin Source File

SOURCE=.\mf_format.c
# End Source File
# Begin Source File

SOURCE=.\mf_loadpath.c
# End Source File
# Begin Source File

SOURCE=.\mf_pack.c
# End Source File
# Begin Source File

SOURCE=.\mf_path.c
# End Source File
# Begin Source File

SOURCE=.\mf_unixpath.c
# End Source File
# Begin Source File

SOURCE=.\mf_wcomp.c
# End Source File
# Begin Source File

SOURCE=.\mulalloc.c
# End Source File
# Begin Source File

SOURCE=.\my_alloc.c
# End Source File
# Begin Source File

SOURCE=.\my_compress.c
# End Source File
# Begin Source File

SOURCE=.\my_create.c
# End Source File
# Begin Source File

SOURCE=.\my_delete.c
# End Source File
# Begin Source File

SOURCE=.\my_div.c
# End Source File
# Begin Source File

SOURCE=.\my_error.c
# End Source File
# Begin Source File

SOURCE=.\my_fopen.c
# End Source File
# Begin Source File

SOURCE=.\my_getwd.c
# End Source File
# Begin Source File

SOURCE=.\my_init.c
# End Source File
# Begin Source File

SOURCE=.\my_lib.c
# End Source File
# Begin Source File

SOURCE=.\my_malloc.c
# End Source File
# Begin Source File

SOURCE=.\my_messnc.c
# End Source File
# Begin Source File

SOURCE=.\my_net.c
# End Source File
# Begin Source File

SOURCE=.\my_once.c
# End Source File
# Begin Source File

SOURCE=.\my_open.c
# End Source File
# Begin Source File

SOURCE=.\my_pthread.c
# End Source File
# Begin Source File

SOURCE=.\my_read.c
# End Source File
# Begin Source File

SOURCE=.\my_realloc.c
# End Source File
# Begin Source File

SOURCE=.\my_static.c
# End Source File
# Begin Source File

SOURCE=.\my_tempnam.c
# End Source File
# Begin Source File

SOURCE=.\my_thr_init.c
# End Source File
# Begin Source File

SOURCE=.\my_wincond.c
# End Source File
# Begin Source File

SOURCE=.\my_winthread.c
# End Source File
# Begin Source File

SOURCE=.\my_write.c
# End Source File
# Begin Source File

SOURCE=.\net.c
# End Source File
# Begin Source File

SOURCE=.\password.c
# End Source File
# Begin Source File

SOURCE=.\safemalloc.c
# End Source File
# Begin Source File

SOURCE=.\str2int.c
# End Source File
# Begin Source File

SOURCE=.\strcend.c
# End Source File
# Begin Source File

SOURCE=.\strcont.c
# End Source File
# Begin Source File

SOURCE=.\strend.c
# End Source File
# Begin Source File

SOURCE=.\strfill.c
# End Source File
# Begin Source File

SOURCE=.\string.c
# End Source File
# Begin Source File

SOURCE=.\strinstr.c
# End Source File
# Begin Source File

SOURCE=.\strmake.c
# End Source File
# Begin Source File

SOURCE=.\strmov.c
# End Source File
# Begin Source File

SOURCE=.\strnmov.c
# End Source File
# Begin Source File

SOURCE=.\strtoll.c
# End Source File
# Begin Source File

SOURCE=.\strtoull.c
# End Source File
# Begin Source File

SOURCE=.\strxmov.c
# End Source File
# Begin Source File

SOURCE=.\thr_mutex.c
# End Source File
# Begin Source File

SOURCE=.\typelib.c
# End Source File
# Begin Source File

SOURCE=.\violite.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\acconfig.h
# End Source File
# Begin Source File

SOURCE=".\config-win.h"
# End Source File
# Begin Source File

SOURCE=.\dbug.h
# End Source File
# Begin Source File

SOURCE=.\errmsg.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\m_ctype.h
# End Source File
# Begin Source File

SOURCE=.\m_string.h
# End Source File
# Begin Source File

SOURCE=.\my_alarm.h
# End Source File
# Begin Source File

SOURCE=.\my_dir.h
# End Source File
# Begin Source File

SOURCE=.\my_list.h
# End Source File
# Begin Source File

SOURCE=.\my_net.h
# End Source File
# Begin Source File

SOURCE=.\my_pthread.h
# End Source File
# Begin Source File

SOURCE=.\my_static.h
# End Source File
# Begin Source File

SOURCE=.\my_sys.h
# End Source File
# Begin Source File

SOURCE=.\mysql.h
# End Source File
# Begin Source File

SOURCE=.\mysql_com.h
# End Source File
# Begin Source File

SOURCE=.\mysql_version.h
# End Source File
# Begin Source File

SOURCE=.\mysqld_error.h
# End Source File
# Begin Source File

SOURCE=.\mysys_err.h
# End Source File
# Begin Source File

SOURCE=.\mysys_priv.h
# End Source File
# Begin Source File

SOURCE=.\thr_alarm.h
# End Source File
# Begin Source File

SOURCE=.\violite.h
# End Source File
# End Group
# End Target
# End Project
