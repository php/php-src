# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=xmltok - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to xmltok - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xmlwf - Win32 Release" && "$(CFG)" != "xmlwf - Win32 Debug" &&\
 "$(CFG)" != "gennmtab - Win32 Release" && "$(CFG)" != "gennmtab - Win32 Debug"\
 && "$(CFG)" != "xmlparse - Win32 Release" && "$(CFG)" !=\
 "xmlparse - Win32 Debug" && "$(CFG)" != "xmltok - Win32 Release" && "$(CFG)" !=\
 "xmltok - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "expat.mak" CFG="xmltok - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xmlwf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xmlwf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gennmtab - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "gennmtab - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "xmlparse - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xmlparse - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xmltok - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xmltok - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "xmltok - Win32 Debug"

!IF  "$(CFG)" == "xmlwf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xmlwf\Release"
# PROP BASE Intermediate_Dir "xmlwf\Release"
# PROP BASE Target_Dir "xmlwf"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xmlwf\Release"
# PROP Intermediate_Dir "xmlwf\Release"
# PROP Target_Dir "xmlwf"
OUTDIR=.\xmlwf\Release
INTDIR=.\xmlwf\Release

ALL : "xmlparse - Win32 Release" ".\bin\xmlwf.exe"

CLEAN : 
	-@erase "$(INTDIR)\codepage.obj"
	-@erase "$(INTDIR)\win32filemap.obj"
	-@erase "$(INTDIR)\xmlwf.obj"
	-@erase ".\bin\xmlwf.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "xmlparse" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D XMLTOKAPI=__declspec(dllimport) /D XMLPARSEAPI=__declspec(dllimport) /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "xmlparse" /D "NDEBUG" /D "WIN32" /D\
 "_CONSOLE" /D XMLTOKAPI=__declspec(dllimport) /D\
 XMLPARSEAPI=__declspec(dllimport) /Fp"$(INTDIR)/xmlwf.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\xmlwf\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmlwf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"bin/xmlwf.exe"
LINK32_FLAGS=setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/xmlwf.pdb" /machine:I386 /out:"bin/xmlwf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\codepage.obj" \
	"$(INTDIR)\win32filemap.obj" \
	"$(INTDIR)\xmlwf.obj" \
	".\xmlparse\Release\xmlparse.lib"

".\bin\xmlwf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xmlwf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xmlwf\Debug"
# PROP BASE Intermediate_Dir "xmlwf\Debug"
# PROP BASE Target_Dir "xmlwf"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xmlwf\Debug"
# PROP Intermediate_Dir "xmlwf\Debug"
# PROP Target_Dir "xmlwf"
OUTDIR=.\xmlwf\Debug
INTDIR=.\xmlwf\Debug

ALL : "xmlparse - Win32 Debug" ".\dbgbin\xmlwf.exe"

CLEAN : 
	-@erase "$(INTDIR)\codepage.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\win32filemap.obj"
	-@erase "$(INTDIR)\xmlwf.obj"
	-@erase "$(OUTDIR)\xmlwf.pdb"
	-@erase ".\dbgbin\xmlwf.exe"
	-@erase ".\dbgbin\xmlwf.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "xmlparse" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D XMLTOKAPI=__declspec(dllimport) /D XMLPARSEAPI=__declspec(dllimport) /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "xmlparse" /D "_DEBUG" /D\
 "WIN32" /D "_CONSOLE" /D XMLTOKAPI=__declspec(dllimport) /D\
 XMLPARSEAPI=__declspec(dllimport) /Fp"$(INTDIR)/xmlwf.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\xmlwf\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmlwf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"dbgbin/xmlwf.exe"
LINK32_FLAGS=setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/xmlwf.pdb" /debug /machine:I386 /out:"dbgbin/xmlwf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\codepage.obj" \
	"$(INTDIR)\win32filemap.obj" \
	"$(INTDIR)\xmlwf.obj" \
	".\xmlparse\Debug\xmlparse.lib"

".\dbgbin\xmlwf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gennmtab - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gennmtab\Release"
# PROP BASE Intermediate_Dir "gennmtab\Release"
# PROP BASE Target_Dir "gennmtab"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "gennmtab\Release"
# PROP Intermediate_Dir "gennmtab\Release"
# PROP Target_Dir "gennmtab"
OUTDIR=.\gennmtab\Release
INTDIR=.\gennmtab\Release

ALL : "$(OUTDIR)\gennmtab.exe"

CLEAN : 
	-@erase "$(INTDIR)\gennmtab.obj"
	-@erase "$(OUTDIR)\gennmtab.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/gennmtab.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\gennmtab\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gennmtab.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/gennmtab.pdb" /machine:I386 /out:"$(OUTDIR)/gennmtab.exe" 
LINK32_OBJS= \
	"$(INTDIR)\gennmtab.obj"

"$(OUTDIR)\gennmtab.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gennmtab - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gennmtab\Debug"
# PROP BASE Intermediate_Dir "gennmtab\Debug"
# PROP BASE Target_Dir "gennmtab"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "gennmtab\Debug"
# PROP Intermediate_Dir "gennmtab\Debug"
# PROP Target_Dir "gennmtab"
OUTDIR=.\gennmtab\Debug
INTDIR=.\gennmtab\Debug

ALL : "$(OUTDIR)\gennmtab.exe"

CLEAN : 
	-@erase "$(INTDIR)\gennmtab.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\gennmtab.exe"
	-@erase "$(OUTDIR)\gennmtab.ilk"
	-@erase "$(OUTDIR)\gennmtab.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/gennmtab.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\gennmtab\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gennmtab.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/gennmtab.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/gennmtab.exe" 
LINK32_OBJS= \
	"$(INTDIR)\gennmtab.obj"

"$(OUTDIR)\gennmtab.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xmlparse - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xmlparse\Release"
# PROP BASE Intermediate_Dir "xmlparse\Release"
# PROP BASE Target_Dir "xmlparse"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xmlparse\Release"
# PROP Intermediate_Dir "xmlparse\Release"
# PROP Target_Dir "xmlparse"
OUTDIR=.\xmlparse\Release
INTDIR=.\xmlparse\Release

ALL : "xmltok - Win32 Release" ".\bin\xmlparse.dll"

CLEAN : 
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(OUTDIR)\xmlparse.exp"
	-@erase "$(OUTDIR)\xmlparse.lib"
	-@erase ".\bin\xmlparse.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "xmltok" /I "xmlwf" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllimport) /D XMLPARSEAPI=__declspec(dllexport) /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "xmltok" /I "xmlwf" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllimport) /D\
 XMLPARSEAPI=__declspec(dllexport) /Fp"$(INTDIR)/xmlparse.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\xmlparse\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmlparse.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /machine:I386 /out:"bin/xmlparse.dll"
# SUBTRACT LINK32 /profile
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/xmlparse.pdb" /machine:I386 /out:"bin/xmlparse.dll"\
 /implib:"$(OUTDIR)/xmlparse.lib" 
LINK32_OBJS= \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\xmlparse.obj" \
	".\xmltok\Release\xmltok.lib"

".\bin\xmlparse.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xmlparse - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xmlparse\Debug"
# PROP BASE Intermediate_Dir "xmlparse\Debug"
# PROP BASE Target_Dir "xmlparse"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xmlparse\Debug"
# PROP Intermediate_Dir "xmlparse\Debug"
# PROP Target_Dir "xmlparse"
OUTDIR=.\xmlparse\Debug
INTDIR=.\xmlparse\Debug

ALL : "xmltok - Win32 Debug" ".\dbgbin\xmlparse.dll"

CLEAN : 
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(OUTDIR)\xmlparse.exp"
	-@erase "$(OUTDIR)\xmlparse.lib"
	-@erase "$(OUTDIR)\xmlparse.pdb"
	-@erase ".\dbgbin\xmlparse.dll"
	-@erase ".\dbgbin\xmlparse.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "xmltok" /I "xmlwf" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllimport) /D XMLPARSEAPI=__declspec(dllexport) /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "xmltok" /I "xmlwf" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllimport) /D\
 XMLPARSEAPI=__declspec(dllexport) /Fp"$(INTDIR)/xmlparse.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\xmlparse\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmlparse.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /debug /machine:I386 /out:"dbgbin/xmlparse.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/xmlparse.pdb" /debug /machine:I386 /out:"dbgbin/xmlparse.dll"\
 /implib:"$(OUTDIR)/xmlparse.lib" 
LINK32_OBJS= \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\xmlparse.obj" \
	".\xmltok\Debug\xmltok.lib"

".\dbgbin\xmlparse.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xmltok - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xmltok\Release"
# PROP BASE Intermediate_Dir "xmltok\Release"
# PROP BASE Target_Dir "xmltok"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xmltok\Release"
# PROP Intermediate_Dir "xmltok\Release"
# PROP Target_Dir "xmltok"
OUTDIR=.\xmltok\Release
INTDIR=.\xmltok\Release

ALL : ".\bin\xmltok.dll"

CLEAN : 
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\xmltok.exp"
	-@erase "$(OUTDIR)\xmltok.lib"
	-@erase ".\bin\xmltok.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllexport) /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 XMLTOKAPI=__declspec(dllexport) /Fp"$(INTDIR)/xmltok.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\xmltok\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmltok.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386 /out:"bin/xmltok.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/xmltok.pdb" /machine:I386 /out:"bin/xmltok.dll"\
 /implib:"$(OUTDIR)/xmltok.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

".\bin\xmltok.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xmltok - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xmltok\Debug"
# PROP BASE Intermediate_Dir "xmltok\Debug"
# PROP BASE Target_Dir "xmltok"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xmltok\Debug"
# PROP Intermediate_Dir "xmltok\Debug"
# PROP Target_Dir "xmltok"
OUTDIR=.\xmltok\Debug
INTDIR=.\xmltok\Debug

ALL : ".\dbgbin\xmltok.dll"

CLEAN : 
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\xmltok.exp"
	-@erase "$(OUTDIR)\xmltok.lib"
	-@erase "$(OUTDIR)\xmltok.pdb"
	-@erase ".\dbgbin\xmltok.dll"
	-@erase ".\dbgbin\xmltok.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D XMLTOKAPI=__declspec(dllexport) /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D XMLTOKAPI=__declspec(dllexport) /Fp"$(INTDIR)/xmltok.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\xmltok\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xmltok.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"dbgbin/xmltok.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/xmltok.pdb" /debug /machine:I386 /out:"dbgbin/xmltok.dll"\
 /implib:"$(OUTDIR)/xmltok.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

".\dbgbin\xmltok.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "xmlwf - Win32 Release"
# Name "xmlwf - Win32 Debug"

!IF  "$(CFG)" == "xmlwf - Win32 Release"

!ELSEIF  "$(CFG)" == "xmlwf - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\xmlwf\xmlwf.c
DEP_CPP_XMLWF=\
	".\xmlparse\xmlparse.h"\
	".\xmlwf\codepage.h"\
	".\xmlwf\filemap.h"\
	

"$(INTDIR)\xmlwf.obj" : $(SOURCE) $(DEP_CPP_XMLWF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmlwf\win32filemap.c
DEP_CPP_WIN32=\
	".\xmlwf\filemap.h"\
	

"$(INTDIR)\win32filemap.obj" : $(SOURCE) $(DEP_CPP_WIN32) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmlwf\unixfilemap.c
DEP_CPP_UNIXF=\
	".\xmlwf\filemap.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmlwf\readfilemap.c
DEP_CPP_READF=\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "xmlparse"

!IF  "$(CFG)" == "xmlwf - Win32 Release"

"xmlparse - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\expat.mak" CFG="xmlparse - Win32 Release" 

!ELSEIF  "$(CFG)" == "xmlwf - Win32 Debug"

"xmlparse - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\expat.mak" CFG="xmlparse - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\xmlwf\codepage.c
DEP_CPP_CODEP=\
	".\xmlwf\codepage.h"\
	

"$(INTDIR)\codepage.obj" : $(SOURCE) $(DEP_CPP_CODEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "gennmtab - Win32 Release"
# Name "gennmtab - Win32 Debug"

!IF  "$(CFG)" == "gennmtab - Win32 Release"

!ELSEIF  "$(CFG)" == "gennmtab - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\gennmtab\gennmtab.c

"$(INTDIR)\gennmtab.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "xmlparse - Win32 Release"
# Name "xmlparse - Win32 Debug"

!IF  "$(CFG)" == "xmlparse - Win32 Release"

!ELSEIF  "$(CFG)" == "xmlparse - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\xmlparse\xmlparse.c

!IF  "$(CFG)" == "xmlparse - Win32 Release"

DEP_CPP_XMLPA=\
	".\xmlparse\hashtable.h"\
	".\xmlparse\xmlparse.h"\
	".\xmltok\xmldef.h"\
	".\xmltok\xmlrole.h"\
	".\xmltok\xmltok.h"\
	
NODEP_CPP_XMLPA=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmlparse.obj" : $(SOURCE) $(DEP_CPP_XMLPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xmlparse - Win32 Debug"

DEP_CPP_XMLPA=\
	".\xmlparse\hashtable.h"\
	".\xmlparse\xmlparse.h"\
	".\xmltok\xmldef.h"\
	".\xmltok\xmlrole.h"\
	".\xmltok\xmltok.h"\
	
NODEP_CPP_XMLPA=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmlparse.obj" : $(SOURCE) $(DEP_CPP_XMLPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmlparse\hashtable.c

!IF  "$(CFG)" == "xmlparse - Win32 Release"

DEP_CPP_HASHT=\
	".\xmlparse\hashtable.h"\
	".\xmltok\xmldef.h"\
	
NODEP_CPP_HASHT=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\hashtable.obj" : $(SOURCE) $(DEP_CPP_HASHT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xmlparse - Win32 Debug"

DEP_CPP_HASHT=\
	".\xmlparse\hashtable.h"\
	".\xmltok\xmldef.h"\
	
NODEP_CPP_HASHT=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\hashtable.obj" : $(SOURCE) $(DEP_CPP_HASHT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "xmltok"

!IF  "$(CFG)" == "xmlparse - Win32 Release"

"xmltok - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\expat.mak" CFG="xmltok - Win32 Release" 

!ELSEIF  "$(CFG)" == "xmlparse - Win32 Debug"

"xmltok - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\expat.mak" CFG="xmltok - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "xmltok - Win32 Release"
# Name "xmltok - Win32 Debug"

!IF  "$(CFG)" == "xmltok - Win32 Release"

!ELSEIF  "$(CFG)" == "xmltok - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\xmltok\xmlrole.c

!IF  "$(CFG)" == "xmltok - Win32 Release"

DEP_CPP_XMLRO=\
	".\xmltok\xmldef.h"\
	".\xmltok\xmlrole.h"\
	".\xmltok\xmltok.h"\
	
NODEP_CPP_XMLRO=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmlrole.obj" : $(SOURCE) $(DEP_CPP_XMLRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xmltok - Win32 Debug"

DEP_CPP_XMLRO=\
	".\xmltok\xmldef.h"\
	".\xmltok\xmlrole.h"\
	".\xmltok\xmltok.h"\
	
NODEP_CPP_XMLRO=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmlrole.obj" : $(SOURCE) $(DEP_CPP_XMLRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmltok\xmltok.c

!IF  "$(CFG)" == "xmltok - Win32 Release"

DEP_CPP_XMLTO=\
	".\xmltok\asciitab.h"\
	".\xmltok\iasciitab.h"\
	".\xmltok\latin1tab.h"\
	".\xmltok\nametab.h"\
	".\xmltok\utf8tab.h"\
	".\xmltok\xmldef.h"\
	".\xmltok\xmltok.h"\
	".\xmltok\xmltok_impl.c"\
	".\xmltok\xmltok_impl.h"\
	
NODEP_CPP_XMLTO=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmltok.obj" : $(SOURCE) $(DEP_CPP_XMLTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xmltok - Win32 Debug"

DEP_CPP_XMLTO=\
	".\xmltok\asciitab.h"\
	".\xmltok\iasciitab.h"\
	".\xmltok\latin1tab.h"\
	".\xmltok\nametab.h"\
	".\xmltok\utf8tab.h"\
	".\xmltok\xmldef.h"\
	".\xmltok\xmltok.h"\
	".\xmltok\xmltok_impl.c"\
	".\xmltok\xmltok_impl.h"\
	
NODEP_CPP_XMLTO=\
	".\xmltok\nspr.h"\
	

"$(INTDIR)\xmltok.obj" : $(SOURCE) $(DEP_CPP_XMLTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xmltok\dllmain.c

!IF  "$(CFG)" == "xmltok - Win32 Release"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xmltok - Win32 Debug"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
