# Microsoft Developer Studio Generated NMAKE File, Based on dbm.dsp
!IF "$(CFG)" == ""
CFG=dbm - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbm - Win32 Release" && "$(CFG)" != "dbm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbm.mak" CFG="dbm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dbm - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

ALL : "$(OUTDIR)\php3_dbm.dll"


CLEAN :
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\flock.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_dbm.dll"
	-@erase "$(OUTDIR)\php3_dbm.exp"
	-@erase "$(OUTDIR)\php3_dbm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D NDBM=1 /D GDBM=0 /D BSD2=1 /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libdb.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_dbm.pdb" /machine:I386 /out:"$(OUTDIR)\php3_dbm.dll" /implib:"$(OUTDIR)\php3_dbm.lib" /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\flock.obj"

"$(OUTDIR)\php3_dbm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbm - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug

ALL : "c:\php3\php3_dbm.dll"


CLEAN :
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\flock.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_dbm.exp"
	-@erase "$(OUTDIR)\php3_dbm.lib"
	-@erase "$(OUTDIR)\php3_dbm.pdb"
	-@erase "c:\php3\php3_dbm.dll"
	-@erase "c:\php3\php3_dbm.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /I "../../include" /D NDBM=1 /D GDBM=0 /D BSD2=1 /D "DEBUG" /D "_DEBUG" /D COMPILE_DL=1 /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libdb.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_dbm.pdb" /debug /machine:I386 /out:"c:\php3/php3_dbm.dll" /implib:"$(OUTDIR)\php3_dbm.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\flock.obj"

"c:\php3\php3_dbm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dbm.dep")
!INCLUDE "dbm.dep"
!ELSE 
!MESSAGE Warning: cannot find "dbm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dbm - Win32 Release" || "$(CFG)" == "dbm - Win32 Debug"
SOURCE=.\functions\db.c

"$(INTDIR)\db.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\win32\flock.c

"$(INTDIR)\flock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

