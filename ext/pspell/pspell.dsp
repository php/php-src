# Microsoft Developer Studio Project File - Name="pspell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pspell - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pspell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pspell.mak" CFG="pspell - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pspell - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pspell - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pspell - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pspell___Win32_Debug_TS"
# PROP BASE Intermediate_Dir "pspell___Win32_Debug_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\zlib" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\zlib" /I "..\..\TSRM" /I "..\..\..\aspell\include\pspell" /I "..\..\..\php_build\include\pspell." /D ZEND_DEBUG=1 /D "_DEBUG" /D ZTS=1 /D HAVE_PSPELL=1 /D "COMPILE_DL_PSPELL" /D "WIN32" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PSPELL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_pspell.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"
# ADD LINK32 php5ts_debug.lib aspell-15.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_pspell.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\aspell\lib"

!ELSEIF  "$(CFG)" == "pspell - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pspell___Win32_Release_TS"
# PROP BASE Intermediate_Dir "pspell___Win32_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PSPELL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\zlib" /I "..\..\TSRM" /I "..\..\..\aspell\include\pspell" /I "..\..\..\php_build\include\pspell" /D ZEND_DEBUG=0 /D "NDEBUG" /D ZTS=1 /D HAVE_PSPELL=1 /D "COMPILE_DL_PSPELL" /D "WIN32" /D "ZEND_WIN32" /D "PHP_WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PSPELL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib aspell-15.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_pspell.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\..\aspell\lib"

!ENDIF 

# Begin Target

# Name "pspell - Win32 Debug_TS"
# Name "pspell - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pspell.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_pspell.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
