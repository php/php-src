# Microsoft Developer Studio Generated NMAKE File, Based on regex.dsp
!IF "$(CFG)" == ""
CFG=regex - Win32 Release
!MESSAGE No configuration specified. Defaulting to regex - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "regex - Win32 Release" && "$(CFG)" != "regex - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "regex.mak" CFG="regex - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "regex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "regex - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\regex.lib"

!ELSE 

ALL : "$(OUTDIR)\regex.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\regcomp.obj"
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regexec.obj"
	-@erase "$(INTDIR)\regfree.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\regex.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\regex.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\regex.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\regex.lib" 
LIB32_OBJS= \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj"

"$(OUTDIR)\regex.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\regex.lib" "$(OUTDIR)\regex.bsc"

!ELSE 

ALL : "$(OUTDIR)\regex.lib" "$(OUTDIR)\regex.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\regcomp.obj"
	-@erase "$(INTDIR)\regcomp.sbr"
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regerror.sbr"
	-@erase "$(INTDIR)\regexec.obj"
	-@erase "$(INTDIR)\regexec.sbr"
	-@erase "$(INTDIR)\regfree.obj"
	-@erase "$(INTDIR)\regfree.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\regex.bsc"
	-@erase "$(OUTDIR)\regex.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "." /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\regex.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\regex.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\regcomp.sbr" \
	"$(INTDIR)\regerror.sbr" \
	"$(INTDIR)\regexec.sbr" \
	"$(INTDIR)\regfree.sbr"

"$(OUTDIR)\regex.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\regex.lib" 
LIB32_OBJS= \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj"

"$(OUTDIR)\regex.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "regex - Win32 Release" || "$(CFG)" == "regex - Win32 Debug"
SOURCE=.\regcomp.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGCO=\
	".\cclass.h"\
	".\cname.h"\
	".\regcomp.ih"\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	

"$(INTDIR)\regcomp.obj" : $(SOURCE) $(DEP_CPP_REGCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGCO=\
	".\cclass.h"\
	".\cname.h"\
	".\regcomp.ih"\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\regcomp.obj"	"$(INTDIR)\regcomp.sbr" : $(SOURCE) $(DEP_CPP_REGCO)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\regerror.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGER=\
	".\regerror.ih"\
	".\regex.h"\
	".\utils.h"\
	

"$(INTDIR)\regerror.obj" : $(SOURCE) $(DEP_CPP_REGER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGER=\
	".\regerror.ih"\
	".\regex.h"\
	".\utils.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\regerror.obj"	"$(INTDIR)\regerror.sbr" : $(SOURCE) $(DEP_CPP_REGER)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\regexec.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGEX=\
	".\engine.c"\
	".\engine.ih"\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	

"$(INTDIR)\regexec.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGEX=\
	".\engine.c"\
	".\engine.ih"\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\regexec.obj"	"$(INTDIR)\regexec.sbr" : $(SOURCE) $(DEP_CPP_REGEX)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\regfree.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGFR=\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	

"$(INTDIR)\regfree.obj" : $(SOURCE) $(DEP_CPP_REGFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGFR=\
	".\regex.h"\
	".\regex2.h"\
	".\utils.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\regfree.obj"	"$(INTDIR)\regfree.sbr" : $(SOURCE) $(DEP_CPP_REGFR)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\engine.c

!ENDIF 

