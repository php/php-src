# Microsoft Developer Studio Generated NMAKE File, Based on mysql.dsp
!IF "$(CFG)" == ""
CFG=mysql - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mysql - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mysql - Win32 Release" && "$(CFG)" != "mysql - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mysql.mak" CFG="mysql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mysql - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mysql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mysql - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

ALL : "$(OUTDIR)\php3_mysql.dll"


CLEAN :
	-@erase "$(INTDIR)\mysql.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\php3_mysql.dll"
	-@erase "$(OUTDIR)\php3_mysql.exp"
	-@erase "$(OUTDIR)\php3_mysql.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D HAVE_MYSQL=1 /D HAVE_ERRMSG_H=1 /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\mysql.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /I /usr/src/phpcvs/php3" /I /usr/src/mysql/include" " " /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mysql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\php3_mysql.pdb" /machine:I386 /out:"$(OUTDIR)\php3_mysql.dll" /implib:"$(OUTDIR)\php3_mysql.lib" /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\mysql.obj"

"$(OUTDIR)\php3_mysql.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mysql - Win32 Debug"

OUTDIR=c:\php3
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=c:\php3
# End Custom Macros

ALL : "$(OUTDIR)\php3_mysql.dll"


CLEAN :
	-@erase "$(INTDIR)\mysql.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\php3_mysql.dll"
	-@erase "$(OUTDIR)\php3_mysql.exp"
	-@erase "$(OUTDIR)\php3_mysql.ilk"
	-@erase "$(OUTDIR)\php3_mysql.lib"
	-@erase "$(OUTDIR)\php3_mysql.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /I "../../include" /D HAVE_MYSQL=1 /D HAVE_ERRMSG_H=1 /D "DEBUG" /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\mysql.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /I /usr/src/phpcvs/php3" /I /usr/src/mysql/include" " " /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mysql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\php3_mysql.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php3_mysql.dll" /implib:"$(OUTDIR)\php3_mysql.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\mysql.obj"

"$(OUTDIR)\php3_mysql.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mysql.dep")
!INCLUDE "mysql.dep"
!ELSE 
!MESSAGE Warning: cannot find "mysql.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mysql - Win32 Release" || "$(CFG)" == "mysql - Win32 Debug"
SOURCE=.\functions\mysql.c

"$(INTDIR)\mysql.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

