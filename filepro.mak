# Microsoft Developer Studio Generated NMAKE File, Based on filepro.dsp
!IF "$(CFG)" == ""
CFG=filepro - Win32 Debug
!MESSAGE No configuration specified. Defaulting to filepro - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "filepro - Win32 Release" && "$(CFG)" != "filepro - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "filepro.mak" CFG="filepro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "filepro - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "filepro - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "filepro - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

ALL : "$(OUTDIR)\php3_filepro.dll"


CLEAN :
	-@erase "$(INTDIR)\filepro.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_filepro.dll"
	-@erase "$(OUTDIR)\php3_filepro.exp"
	-@erase "$(OUTDIR)\php3_filepro.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D HAVE_FILEPRO=1 /D "NDEBUG" /D "THREAD_SAFE" /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\filepro.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\filepro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.0 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_filepro.pdb" /machine:I386 /out:"$(OUTDIR)\php3_filepro.dll" /implib:"$(OUTDIR)\php3_filepro.lib" /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\filepro.obj"

"$(OUTDIR)\php3_filepro.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "filepro - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

ALL : "$(OUTDIR)\php3_filepro.dll"


CLEAN :
	-@erase "$(INTDIR)\filepro.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_filepro.dll"
	-@erase "$(OUTDIR)\php3_filepro.exp"
	-@erase "$(OUTDIR)\php3_filepro.ilk"
	-@erase "$(OUTDIR)\php3_filepro.lib"
	-@erase "$(OUTDIR)\php3_filepro.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /I "../../include" /D HAVE_FILEPRO=1 /D "DEBUG" /D "_DEBUG" /D "THREAD_SAFE" /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\filepro.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\filepro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.0 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_filepro.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_filepro.dll" /implib:"$(OUTDIR)\php3_filepro.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\filepro.obj"

"$(OUTDIR)\php3_filepro.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("filepro.dep")
!INCLUDE "filepro.dep"
!ELSE 
!MESSAGE Warning: cannot find "filepro.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "filepro - Win32 Release" || "$(CFG)" == "filepro - Win32 Debug"
SOURCE=.\functions\filepro.c

"$(INTDIR)\filepro.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

