# Microsoft Developer Studio Generated NMAKE File, Based on hyperwave.dsp
!IF "$(CFG)" == ""
CFG=hyperwave - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hyperwave - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hyperwave - Win32 Release" && "$(CFG)" != "hyperwave - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hyperwave.mak" CFG="hyperwave - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hyperwave - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hyperwave - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "hyperwave - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

ALL : "$(OUTDIR)\php3_hyperwave.dll"


CLEAN :
	-@erase "$(INTDIR)\dlist.obj"
	-@erase "$(INTDIR)\hg_comm.obj"
	-@erase "$(INTDIR)\hw.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_hyperwave.dll"
	-@erase "$(OUTDIR)\php3_hyperwave.exp"
	-@erase "$(OUTDIR)\php3_hyperwave.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D "HYPERWAVE" /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\hyperwave.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hyperwave.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_hyperwave.pdb" /machine:I386 /out:"$(OUTDIR)\php3_hyperwave.dll" /implib:"$(OUTDIR)\php3_hyperwave.lib" /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\dlist.obj" \
	"$(INTDIR)\hg_comm.obj" \
	"$(INTDIR)\hw.obj"

"$(OUTDIR)\php3_hyperwave.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hyperwave - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

ALL : "$(OUTDIR)\php3_hyperwave.dll"


CLEAN :
	-@erase "$(INTDIR)\dlist.obj"
	-@erase "$(INTDIR)\hg_comm.obj"
	-@erase "$(INTDIR)\hw.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_hyperwave.dll"
	-@erase "$(OUTDIR)\php3_hyperwave.exp"
	-@erase "$(OUTDIR)\php3_hyperwave.ilk"
	-@erase "$(OUTDIR)\php3_hyperwave.lib"
	-@erase "$(OUTDIR)\php3_hyperwave.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /I "../../include" /D "HYPERWAVE" /D "DEBUG" /D "_DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\hyperwave.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hyperwave.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_hyperwave.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_hyperwave.dll" /implib:"$(OUTDIR)\php3_hyperwave.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\dlist.obj" \
	"$(INTDIR)\hg_comm.obj" \
	"$(INTDIR)\hw.obj"

"$(OUTDIR)\php3_hyperwave.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hyperwave.dep")
!INCLUDE "hyperwave.dep"
!ELSE 
!MESSAGE Warning: cannot find "hyperwave.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hyperwave - Win32 Release" || "$(CFG)" == "hyperwave - Win32 Debug"
SOURCE=.\functions\dlist.c

"$(INTDIR)\dlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\functions\hg_comm.c

"$(INTDIR)\hg_comm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\functions\hw.c

"$(INTDIR)\hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

