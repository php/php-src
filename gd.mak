# Microsoft Developer Studio Generated NMAKE File, Based on gd.dsp
!IF "$(CFG)" == ""
CFG=gd - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gd - Win32 Release" && "$(CFG)" != "gd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gd.mak" CFG="gd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gd - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gd - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "gd - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

ALL : "$(OUTDIR)\php3_gd.dll"


CLEAN :
	-@erase "$(INTDIR)\gd.obj"
	-@erase "$(INTDIR)\gdcache.obj"
	-@erase "$(INTDIR)\gdttf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_gd.dll"
	-@erase "$(OUTDIR)\php3_gd.exp"
	-@erase "$(OUTDIR)\php3_gd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBTTF=1 /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\gd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=freetype.lib php.lib libgd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_gd.pdb" /machine:I386 /out:"$(OUTDIR)\php3_gd.dll" /implib:"$(OUTDIR)\php3_gd.lib" /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\gd.obj" \
	"$(INTDIR)\gdcache.obj" \
	"$(INTDIR)\gdttf.obj"

"$(OUTDIR)\php3_gd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gd - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

ALL : "$(OUTDIR)\php3_gd.dll"


CLEAN :
	-@erase "$(INTDIR)\gd.obj"
	-@erase "$(INTDIR)\gdcache.obj"
	-@erase "$(INTDIR)\gdttf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_gd.dll"
	-@erase "$(OUTDIR)\php3_gd.exp"
	-@erase "$(OUTDIR)\php3_gd.ilk"
	-@erase "$(OUTDIR)\php3_gd.lib"
	-@erase "$(OUTDIR)\php3_gd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /I "../../include" /D HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBTTF=1 /D "DEBUG" /D "_DEBUG" /D "HAVE_GDTTF" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\gd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=freetype.lib php.lib libgd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_gd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_gd.dll" /implib:"$(OUTDIR)\php3_gd.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\gd.obj" \
	"$(INTDIR)\gdcache.obj" \
	"$(INTDIR)\gdttf.obj"

"$(OUTDIR)\php3_gd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("gd.dep")
!INCLUDE "gd.dep"
!ELSE 
!MESSAGE Warning: cannot find "gd.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gd - Win32 Release" || "$(CFG)" == "gd - Win32 Debug"
SOURCE=.\functions\gd.c

"$(INTDIR)\gd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\functions\gdcache.c

"$(INTDIR)\gdcache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\functions\gdttf.c

"$(INTDIR)\gdttf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

