# Microsoft Developer Studio Project File - Name="sqlite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sqlite - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sqlite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sqlite.mak" CFG="sqlite - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sqlite - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sqlite - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sqlite - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SQLITE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\win32" /I "..\..\..\php_build" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_SQLITE" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SQLITE=1 /D "PHP_SQLITE_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS\php_sqlite.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline" /libpath:"..\..\..\php_build\release"

!ELSEIF  "$(CFG)" == "sqlite - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SQLITE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\win32" /I "..\..\..\php_build" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_SQLITE" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_SQLITE=1 /D "PHP_SQLITE_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php5ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS\php_sqlite.dll" /pdbtype:sept /libpath:"..\..\Debug_TS" /libpath:"..\..\..\php_build\release"

!ENDIF 

# Begin Target

# Name "sqlite - Win32 Release_TS"
# Name "sqlite - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "libsqlite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libsqlite\src\attach.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\auth.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\btree.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\btree.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\btree_rb.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\build.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\config.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\copy.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\date.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\delete.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\encode.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\expr.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\func.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\hash.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\hash.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\insert.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\main.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\opcodes.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\os.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\os.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\pager.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\pager.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\parse.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\parse.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\pragma.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\printf.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\random.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\select.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\sqlite.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\sqlite.w32.h

!IF  "$(CFG)" == "sqlite - Win32 Release_TS"

# Begin Custom Build
InputDir=.\libsqlite\src
InputPath=.\libsqlite\src\sqlite.w32.h

"$(InputDir)\sqlite.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\sqlite.h

# End Custom Build

!ELSEIF  "$(CFG)" == "sqlite - Win32 Debug_TS"

# Begin Custom Build
InputDir=.\libsqlite\src
InputPath=.\libsqlite\src\sqlite.w32.h

"$(InputDir)\sqlite.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\sqlite.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\sqlite_config.w32.h

!IF  "$(CFG)" == "sqlite - Win32 Release_TS"

# Begin Custom Build
InputDir=.\libsqlite\src
InputPath=.\libsqlite\src\sqlite_config.w32.h

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "sqlite - Win32 Debug_TS"

# Begin Custom Build
InputDir=.\libsqlite\src
InputPath=.\libsqlite\src\sqlite_config.w32.h

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\sqliteInt.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\table.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\tokenize.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\trigger.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\update.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\util.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\vacuum.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\vdbe.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\vdbe.h
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\vdbeaux.c
# End Source File
# Begin Source File

SOURCE=.\libsqlite\src\where.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\php_sqlite.def
# End Source File
# Begin Source File

SOURCE=.\sqlite.c
# ADD CPP /I "libsqlite\src"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_sqlite.h
# End Source File
# End Group
# End Target
# End Project
