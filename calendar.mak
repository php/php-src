# Microsoft Developer Studio Generated NMAKE File, Based on calendar.dsp
!IF "$(CFG)" == ""
CFG=calendar - Win32 Debug
!MESSAGE No configuration specified. Defaulting to calendar - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "calendar - Win32 Release" && "$(CFG)" != "calendar - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "calendar.mak" CFG="calendar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "calendar - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "calendar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "calendar - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

ALL : "$(OUTDIR)\php3_calendar.dll"


CLEAN :
	-@erase "$(INTDIR)\calendar.obj"
	-@erase "$(INTDIR)\dow.obj"
	-@erase "$(INTDIR)\french.obj"
	-@erase "$(INTDIR)\gregor.obj"
	-@erase "$(INTDIR)\jewish.obj"
	-@erase "$(INTDIR)\julian.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_calendar.dll"
	-@erase "$(OUTDIR)\php3_calendar.exp"
	-@erase "$(OUTDIR)\php3_calendar.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\calendar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_calendar.pdb" /machine:I386 /out:"$(OUTDIR)\php3_calendar.dll" /implib:"$(OUTDIR)\php3_calendar.lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\php3_calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

ALL : "$(OUTDIR)\php3_calendar.dll"


CLEAN :
	-@erase "$(INTDIR)\calendar.obj"
	-@erase "$(INTDIR)\dow.obj"
	-@erase "$(INTDIR)\french.obj"
	-@erase "$(INTDIR)\gregor.obj"
	-@erase "$(INTDIR)\jewish.obj"
	-@erase "$(INTDIR)\julian.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_calendar.dll"
	-@erase "$(OUTDIR)\php3_calendar.exp"
	-@erase "$(OUTDIR)\php3_calendar.ilk"
	-@erase "$(OUTDIR)\php3_calendar.lib"
	-@erase "$(OUTDIR)\php3_calendar.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /D "DEBUG" /D "_DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\calendar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_calendar.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_calendar.dll" /implib:"$(OUTDIR)\php3_calendar.lib" /pdbtype:sept /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\php3_calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("calendar.dep")
!INCLUDE "calendar.dep"
!ELSE 
!MESSAGE Warning: cannot find "calendar.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "calendar - Win32 Release" || "$(CFG)" == "calendar - Win32 Debug"
SOURCE=.\dl\calendar\calendar.c

"$(INTDIR)\calendar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dl\calendar\dow.c

"$(INTDIR)\dow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dl\calendar\french.c

"$(INTDIR)\french.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dl\calendar\gregor.c

"$(INTDIR)\gregor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dl\calendar\jewish.c

"$(INTDIR)\jewish.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dl\calendar\julian.c

"$(INTDIR)\julian.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

