# Microsoft Developer Studio Project File - Name="libsqlite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libsqlite - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libsqlite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libsqlite.mak" CFG="libsqlite - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libsqlite - Win32 Debug_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "libsqlite - Win32 Release_TS" (based on "Win32 (x86) Static Library")
!MESSAGE "libsqlite - Win32 Release_TSDbg" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libsqlite - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug_TS"
# PROP Intermediate_Dir "..\..\Debug_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D THREADSAFE=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TS"
# PROP Intermediate_Dir "..\..\Release_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D THREADSAFE=1 /YX /FD /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libsqlite___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "libsqlite___Win32_Release_TSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release_TSDbg"
# PROP Intermediate_Dir "..\..\Release_TSDbg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D THREADSAFE=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D THREADSAFE=1 /YX /FD /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release_TS\libsqlite.lib"
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libsqlite - Win32 Debug_TS"
# Name "libsqlite - Win32 Release_TS"
# Name "libsqlite - Win32 Release_TSDbg"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=attach.c
# End Source File
# Begin Source File

SOURCE=auth.c
# End Source File
# Begin Source File

SOURCE=btree.c
# End Source File
# Begin Source File

SOURCE=btree_rb.c
# End Source File
# Begin Source File

SOURCE=build.c
# End Source File
# Begin Source File

SOURCE=copy.c
# End Source File
# Begin Source File

SOURCE=.\date.c
# End Source File
# Begin Source File

SOURCE=delete.c
# End Source File
# Begin Source File

SOURCE=encode.c
# End Source File
# Begin Source File

SOURCE=expr.c
# End Source File
# Begin Source File

SOURCE=func.c
# End Source File
# Begin Source File

SOURCE=hash.c
# End Source File
# Begin Source File

SOURCE=insert.c
# End Source File
# Begin Source File

SOURCE=main.c
# End Source File
# Begin Source File

SOURCE=opcodes.c
# End Source File
# Begin Source File

SOURCE=os.c
# End Source File
# Begin Source File

SOURCE=pager.c
# End Source File
# Begin Source File

SOURCE=parse.c
# End Source File
# Begin Source File

SOURCE=pragma.c
# End Source File
# Begin Source File

SOURCE=printf.c
# End Source File
# Begin Source File

SOURCE=random.c
# End Source File
# Begin Source File

SOURCE=select.c
# End Source File
# Begin Source File

SOURCE=table.c
# End Source File
# Begin Source File

SOURCE=tokenize.c
# End Source File
# Begin Source File

SOURCE=trigger.c
# End Source File
# Begin Source File

SOURCE=update.c
# End Source File
# Begin Source File

SOURCE=util.c
# End Source File
# Begin Source File

SOURCE=vacuum.c
# End Source File
# Begin Source File

SOURCE=vdbe.c
# End Source File
# Begin Source File

SOURCE=.\vdbeaux.c
# End Source File
# Begin Source File

SOURCE=where.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=btree.h
# End Source File
# Begin Source File

SOURCE=config_static.w32.h

!IF  "$(CFG)" == "libsqlite - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=config_static.w32.h

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=config_static.w32.h

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TSDbg"

# Begin Custom Build
InputDir=.
InputPath=config_static.w32.h

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=hash.h
# End Source File
# Begin Source File

SOURCE=opcodes.h
# End Source File
# Begin Source File

SOURCE=os.h
# End Source File
# Begin Source File

SOURCE=pager.h
# End Source File
# Begin Source File

SOURCE=parse.h
# End Source File
# Begin Source File

SOURCE=sqlite.w32.h

!IF  "$(CFG)" == "libsqlite - Win32 Debug_TS"

# Begin Custom Build
InputDir=.
InputPath=sqlite.w32.h

"$(InputDir)\sqlite.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\sqlite.h

# End Custom Build

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TS"

# Begin Custom Build
InputDir=.
InputPath=sqlite.w32.h

"$(InputDir)\sqlite.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\sqlite.h

# End Custom Build

!ELSEIF  "$(CFG)" == "libsqlite - Win32 Release_TSDbg"

# Begin Custom Build
InputDir=.
InputPath=sqlite.w32.h

"$(InputDir)\sqlite.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\sqlite.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=sqliteInt.h
# End Source File
# Begin Source File

SOURCE=vdbe.h
# End Source File
# End Group
# End Target
# End Project
