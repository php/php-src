# Microsoft Developer Studio Generated NMAKE File, Based on dbase.dsp
!IF "$(CFG)" == ""
CFG=dbase - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbase - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbase - Win32 Release" && "$(CFG)" != "dbase - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbase.mak" CFG="dbase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

ALL : "$(OUTDIR)\php3_dbase.dll"


CLEAN :
	-@erase "$(INTDIR)\dbase.obj"
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_dbase.dll"
	-@erase "$(OUTDIR)\php3_dbase.exp"
	-@erase "$(OUTDIR)\php3_dbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "dbase\\" /I "./" /I "../" /D DBASE=1 /D "NDEBUG" /D "THREAD_SAFE" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_dbase.pdb" /machine:I386 /out:"$(OUTDIR)\php3_dbase.dll" /implib:"$(OUTDIR)\php3_dbase.lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\dbase.obj" \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\php3_dbase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

ALL : "$(OUTDIR)\php3_dbase.dll"


CLEAN :
	-@erase "$(INTDIR)\dbase.obj"
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_dbase.dll"
	-@erase "$(OUTDIR)\php3_dbase.exp"
	-@erase "$(OUTDIR)\php3_dbase.ilk"
	-@erase "$(OUTDIR)\php3_dbase.lib"
	-@erase "$(OUTDIR)\php3_dbase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "dbase\\" /I "./" /I "../" /D DBASE=1 /D "DEBUG" /D "_DEBUG" /D "THREAD_SAFE" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_dbase.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_dbase.dll" /implib:"$(OUTDIR)\php3_dbase.lib" /pdbtype:sept /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\dbase.obj" \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\php3_dbase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dbase.dep")
!INCLUDE "dbase.dep"
!ELSE 
!MESSAGE Warning: cannot find "dbase.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dbase - Win32 Release" || "$(CFG)" == "dbase - Win32 Debug"
SOURCE=.\functions\dbase.c

"$(INTDIR)\dbase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dbase\dbf_head.c

"$(INTDIR)\dbf_head.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dbase\dbf_misc.c

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dbase\dbf_ndx.c

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dbase\dbf_rec.c

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

