# Microsoft Developer Studio Generated NMAKE File, Based on ZendCore.dsp
!IF "$(CFG)" == ""
CFG=ZendCore - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ZendCore - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ZendCore - Win32 Release" && "$(CFG)" != "ZendCore - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ZendCore.mak" CFG="ZendCore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZendCore - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ZendCore - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ZendCore - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ZendCore.lib"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\compile.obj"
	-@erase "$(INTDIR)\constants.obj"
	-@erase "$(INTDIR)\execute.obj"
	-@erase "$(INTDIR)\highlight.obj"
	-@erase "$(INTDIR)\llist.obj"
	-@erase "$(INTDIR)\opcode.obj"
	-@erase "$(INTDIR)\operators.obj"
	-@erase "$(INTDIR)\ptr_stack.obj"
	-@erase "$(INTDIR)\sprintf.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zend.obj"
	-@erase "$(INTDIR)\zend_API.obj"
	-@erase "$(INTDIR)\zend_hash.obj"
	-@erase "$(INTDIR)\zend_ini.obj"
	-@erase "$(OUTDIR)\ZendCore.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\ZendCore.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ZendCore.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ZendCore.lib" 
LIB32_OBJS= \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\compile.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\execute.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\llist.obj" \
	"$(INTDIR)\opcode.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\ptr_stack.obj" \
	"$(INTDIR)\sprintf.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\zend.obj" \
	"$(INTDIR)\zend_hash.obj" \
	"$(INTDIR)\zend_ini.obj" \
	"$(INTDIR)\zend_API.obj"

"$(OUTDIR)\ZendCore.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\zend-scanner.c" ".\zend-parser.tab.h" ".\zend-parser.tab.c" ".\configuration-scanner.c" ".\configuration-parser.tab.h" ".\configuration-parser.tab.c" "$(OUTDIR)\ZendCore.lib" "$(OUTDIR)\ZendCore.bsc"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\alloc.sbr"
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\alloca.sbr"
	-@erase "$(INTDIR)\compile.obj"
	-@erase "$(INTDIR)\compile.sbr"
	-@erase "$(INTDIR)\constants.obj"
	-@erase "$(INTDIR)\constants.sbr"
	-@erase "$(INTDIR)\execute.obj"
	-@erase "$(INTDIR)\execute.sbr"
	-@erase "$(INTDIR)\highlight.obj"
	-@erase "$(INTDIR)\highlight.sbr"
	-@erase "$(INTDIR)\llist.obj"
	-@erase "$(INTDIR)\llist.sbr"
	-@erase "$(INTDIR)\opcode.obj"
	-@erase "$(INTDIR)\opcode.sbr"
	-@erase "$(INTDIR)\operators.obj"
	-@erase "$(INTDIR)\operators.sbr"
	-@erase "$(INTDIR)\ptr_stack.obj"
	-@erase "$(INTDIR)\ptr_stack.sbr"
	-@erase "$(INTDIR)\sprintf.obj"
	-@erase "$(INTDIR)\sprintf.sbr"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\stack.sbr"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\variables.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zend.obj"
	-@erase "$(INTDIR)\zend.sbr"
	-@erase "$(INTDIR)\zend_API.obj"
	-@erase "$(INTDIR)\zend_API.sbr"
	-@erase "$(INTDIR)\zend_hash.obj"
	-@erase "$(INTDIR)\zend_hash.sbr"
	-@erase "$(INTDIR)\zend_ini.obj"
	-@erase "$(INTDIR)\zend_ini.sbr"
	-@erase "$(OUTDIR)\ZendCore.bsc"
	-@erase "$(OUTDIR)\ZendCore.lib"
	-@erase "configuration-parser.tab.c"
	-@erase "configuration-parser.tab.h"
	-@erase "configuration-scanner.c"
	-@erase "zend-parser.tab.c"
	-@erase "zend-parser.tab.h"
	-@erase "zend-scanner.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ZendCore.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ZendCore.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\alloc.sbr" \
	"$(INTDIR)\alloca.sbr" \
	"$(INTDIR)\compile.sbr" \
	"$(INTDIR)\constants.sbr" \
	"$(INTDIR)\execute.sbr" \
	"$(INTDIR)\highlight.sbr" \
	"$(INTDIR)\llist.sbr" \
	"$(INTDIR)\opcode.sbr" \
	"$(INTDIR)\operators.sbr" \
	"$(INTDIR)\ptr_stack.sbr" \
	"$(INTDIR)\sprintf.sbr" \
	"$(INTDIR)\stack.sbr" \
	"$(INTDIR)\variables.sbr" \
	"$(INTDIR)\zend.sbr" \
	"$(INTDIR)\zend_hash.sbr" \
	"$(INTDIR)\zend_ini.sbr" \
	"$(INTDIR)\zend_API.sbr"

"$(OUTDIR)\ZendCore.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ZendCore.lib" 
LIB32_OBJS= \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\compile.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\execute.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\llist.obj" \
	"$(INTDIR)\opcode.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\ptr_stack.obj" \
	"$(INTDIR)\sprintf.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\zend.obj" \
	"$(INTDIR)\zend_hash.obj" \
	"$(INTDIR)\zend_ini.obj" \
	"$(INTDIR)\zend_API.obj"

"$(OUTDIR)\ZendCore.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ZendCore.dep")
!INCLUDE "ZendCore.dep"
!ELSE 
!MESSAGE Warning: cannot find "ZendCore.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ZendCore - Win32 Release" || "$(CFG)" == "ZendCore - Win32 Debug"
SOURCE=.\zend_alloc.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\alloc.obj"	"$(INTDIR)\alloc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\alloca.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\alloca.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\alloca.obj"	"$(INTDIR)\alloca.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_compile.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\compile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\compile.obj"	"$(INTDIR)\compile.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_constants.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\constants.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\constants.obj"	"$(INTDIR)\constants.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_execute.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\execute.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\execute.obj"	"$(INTDIR)\execute.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_highlight.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\highlight.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\highlight.obj"	"$(INTDIR)\highlight.sbr" : $(SOURCE) "$(INTDIR)" ".\zend-parser.tab.h"


!ENDIF 

SOURCE=.\zend_llist.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\llist.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\llist.obj"	"$(INTDIR)\llist.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_opcode.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\opcode.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\opcode.obj"	"$(INTDIR)\opcode.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_operators.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\operators.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\operators.obj"	"$(INTDIR)\operators.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_ptr_stack.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\ptr_stack.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\ptr_stack.obj"	"$(INTDIR)\ptr_stack.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sprintf.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\sprintf.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\sprintf.obj"	"$(INTDIR)\sprintf.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_stack.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\stack.obj"	"$(INTDIR)\stack.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_variables.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\variables.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\variables.obj"	"$(INTDIR)\variables.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\zend.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\zend.obj"	"$(INTDIR)\zend.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_API.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\zend_API.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\zend_API.obj"	"$(INTDIR)\zend_API.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_hash.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\zend_hash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\zend_hash.obj"	"$(INTDIR)\zend_hash.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zend_ini.c

!IF  "$(CFG)" == "ZendCore - Win32 Release"


"$(INTDIR)\zend_ini.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"


"$(INTDIR)\zend_ini.obj"	"$(INTDIR)\zend_ini.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\configuration-parser.y"

!IF  "$(CFG)" == "ZendCore - Win32 Release"

!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"

InputPath=".\configuration-parser.y"

".\configuration-parser.tab.c"	".\configuration-parser.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	bison -d -S "C:\Program Files\Cygnus\share\bison.simple" -p cfg configuration-parser.y
<< 
	

!ENDIF 

SOURCE=".\zend-parser.y"

!IF  "$(CFG)" == "ZendCore - Win32 Release"

!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"

InputPath=".\zend-parser.y"

".\zend-parser.tab.c"	".\zend-parser.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	bison -v -d -S "C:\Program Files\Cygnus\share\bison.simple" -p zend zend-parser.y
<< 
	

!ENDIF 

SOURCE=".\configuration-scanner.l"

!IF  "$(CFG)" == "ZendCore - Win32 Release"

!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"

InputPath=".\configuration-scanner.l"

".\configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	flex -i -Pcfg -oconfiguration-scanner.c configuration-scanner.l
<< 
	

!ENDIF 

SOURCE=".\zend-scanner.l"

!IF  "$(CFG)" == "ZendCore - Win32 Release"

!ELSEIF  "$(CFG)" == "ZendCore - Win32 Debug"

InputPath=".\zend-scanner.l"

".\zend-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	flex -i -Pzend -ozend-scanner.c zend-scanner.l
<< 
	

!ENDIF 


!ENDIF 

