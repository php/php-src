# Microsoft Developer Studio Project File - Name="gd_bundled" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gd_bundled - Win32 Release_TS GD2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gd_bundled.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gd_bundled.mak" CFG="gd_bundled - Win32 Release_TS GD2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gd_bundled - Win32 Release_TS GD2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gd_bundled - Win32 Debug_TS GD2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gd___Win32_Release_TS_GD2_bundled"
# PROP BASE Intermediate_Dir "gd___Win32_Release_TS_GD2_bundled"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_GD" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_GD_GIF_READ=1 /D HAVE_GDIMAGECOLORRESOLVE=1 /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /D "HAVE_GD_WBMP" /D HAVE_LIBGD13=1 /D HAVE_LIBGD=1 /D HAVE_LIBGD15=1 /D HAVE_LIBGD204=1 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "libgd" /D ZEND_DEBUG=0 /D HAVE_LIBGD15=1 /D HAVE_LIBGD204=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_GD" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_GD_GIF_READ=1 /D HAVE_GDIMAGECOLORRESOLVE=1 /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /D "HAVE_GD_WBMP" /D HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBGD20=1 /D "USE_GD_IOCTX" /D HAVE_LIBFREETYPE=1 /D "USE_GD_IMGSTRTTF" /D HAVE_GD_STRINGTTF=1 /D HAVE_GD_BUNDLED=1 /D "MSWIN32" /D "HAVE_LIBPNG" /D "HAVE_LIBJPEG" /D "HAVE_GD_GD2" /D "HAVE_GD_STRINGFTEX" /D "HAVE_GD_IMAGESETBRUSH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php4ts.lib libjpeg.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_gd2.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 php4ts.lib freetype2.lib libjpeg.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_gd2.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gd___Win32_Debug_TS_GD2_bundled"
# PROP BASE Intermediate_Dir "gd___Win32_Debug_TS_GD2_bundled"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_GD" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_GD_GIF_READ=1 /D HAVE_GDIMAGECOLORRESOLVE=1 /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /D "HAVE_GD_WBMP" /D HAVE_LIBGD13=1 /D HAVE_LIBGD=1 /D HAVE_LIBGD15=1 /D HAVE_LIBGD204=1 /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\TSRM" /I "libgd" /D ZEND_DEBUG=1 /D "HAVE_LIBGD15" /D HAVE_LIBGD204=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_GD" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_GD_GIF_READ=1 /D HAVE_GDIMAGECOLORRESOLVE=1 /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /D "HAVE_GD_WBMP" /D HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBGD20=1 /D "USE_GD_IOCTX" /D HAVE_LIBFREETYPE=1 /D "USE_GD_IMGSTRTTF" /D HAVE_GD_STRINGTTF=1 /D HAVE_GD_BUNDLED=1 /D "MSWIN32" /D "HAVE_LIBPNG" /D "HAVE_LIBJPEG" /D "HAVE_GD_GD2" /D "HAVE_GD_STRINGFTEX" /D "HAVE_GD_IMAGESETBRUSH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php4ts_debug.lib libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386 /out:"..\..\Debug_TS/php_gd2.dll" /libpath:"..\..\Debug_TS"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 php4ts_debug.lib libpng.lib zlib.lib libjpeg.lib freetype2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386 /out:"..\..\Debug_TS/php_gd2.dll" /libpath:"..\..\Debug_TS"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "gd_bundled - Win32 Release_TS GD2"
# Name "gd_bundled - Win32 Debug_TS GD2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gd.c
# End Source File
# Begin Source File

SOURCE=.\gdttf.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gdcache.h
# End Source File
# Begin Source File

SOURCE=.\gdttf.h
# End Source File
# Begin Source File

SOURCE=.\php_gd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libgd"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\libgd\gd.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd2copypal.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_arc_f_buggy.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_gd.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_gd2.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_gif_in.c
# End Source File
# Begin Source File

SOURCE=.\libgd\gd_io.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_io_dp.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_io_file.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_io_ss.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_jpeg.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_png.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_ss.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_topal.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gd_wbmp.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdcache.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontg.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontl.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontmb.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdfonts.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontt.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdft.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdhelpers.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdkanji.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdtables.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\gdxpm.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libgd\wbmp.c

!IF  "$(CFG)" == "gd_bundled - Win32 Release_TS GD2"

# PROP Intermediate_Dir "Release_TS_bundled"

!ELSEIF  "$(CFG)" == "gd_bundled - Win32 Debug_TS GD2"

# PROP Intermediate_Dir "Debug_TS_bundled"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\libgd\gd.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gd_io.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdcache.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontg.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontl.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontmb.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdfonts.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdfontt.h
# End Source File
# Begin Source File

SOURCE=.\libgd\gdhelpers.h
# End Source File
# Begin Source File

SOURCE=.\libgd\jisx0208.h
# End Source File
# Begin Source File

SOURCE=.\libgd\wbmp.h
# End Source File
# End Group
# End Group
# End Target
# End Project
