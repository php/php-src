# Microsoft Developer Studio Project File - Name="libbcmath" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libbcmath - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbcmath.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbcmath.mak" CFG="libbcmath - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbcmath - Win32 Release_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "libbcmath - Win32 Debug_TS" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbcmath - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "libbcmath_Release_TS"
# PROP BASE Intermediate_Dir "libbcmath_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "libbcmath_Release_TS"
# PROP Intermediate_Dir "libbcmath_Release_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDebug_TS" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../ext/bcmath/libbcmath/src" /I "../Zend" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDebug_TS"
# ADD RSC /l 0x409 /d "NDebug_TS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libbcmath - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "libbcmath_Debug_TS"
# PROP BASE Intermediate_Dir "libbcmath_Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "libbcmath_Debug_TS"
# PROP Intermediate_Dir "libbcmath_Debug_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_Debug_TS" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_Debug_TS" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_Debug_TS"
# ADD RSC /l 0x409 /d "_Debug_TS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libbcmath - Win32 Release_TS"
# Name "libbcmath - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\add.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\compare.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\debug.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\div.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\divmod.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\doaddsub.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\init.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\int2num.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\nearzero.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\neg.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2long.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2str.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\outofmem.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\output.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raise.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raisemod.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\recmul.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rmzero.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rt.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sqrt.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\str2num.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sub.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\zero.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
