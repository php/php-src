# Microsoft Developer Studio Project File - Name="Zend" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Zend - Win32 Release_inline
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Zend.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Zend.mak" CFG="Zend - Win32 Release_inline"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Zend - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Zend - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Zend - Win32 Release_inline" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Zend - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDebug" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDebug" /D "_LIB" /D "Zend_EXPORTS" /D ZEND_DEBUG=0 /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /D "ZEND_WIN32" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40d /d "NDebug"
# ADD RSC /l 0x40d /d "NDebug"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Zend - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_Debug" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "_Debug" /D "_LIB" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D ZEND_DEBUG=1 /D "ZEND_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x40d /d "_Debug"
# ADD RSC /l 0x40d /d "_Debug"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Zend - Win32 Release_inline"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Zend___Win32_Release_inline"
# PROP BASE Intermediate_Dir "Zend___Win32_Release_inline"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Release_inline"
# PROP Intermediate_Dir "Release_inline"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDebug" /D "_LIB" /D "Zend_EXPORTS" /D ZEND_DEBUG=0 /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "WIN32" /D "_MBCS" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "NDebug" /D "_LIB" /D "Zend_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D ZEND_DEBUG=0 /D "ZEND_WIN32_FORCE_INLINE" /D "WIN32" /D "_MBCS" /D "ZEND_WIN32" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40d /d "NDebug"
# ADD RSC /l 0x40d /d "NDebug"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Zend - Win32 Release"
# Name "Zend - Win32 Debug"
# Name "Zend - Win32 Release_inline"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\zend.c
# End Source File
# Begin Source File

SOURCE=.\zend_alloc.c
# End Source File
# Begin Source File

SOURCE=.\zend_API.c
# End Source File
# Begin Source File

SOURCE=.\zend_ast.c
# End Source File
# Begin Source File

SOURCE=.\zend_builtin_functions.c
# End Source File
# Begin Source File

SOURCE=.\zend_closures.c
# End Source File
# Begin Source File

SOURCE=.\zend_compile.c
# End Source File
# Begin Source File

SOURCE=.\zend_constants.c
# End Source File
# Begin Source File

SOURCE=.\zend_default_classes.c
# End Source File
# Begin Source File

SOURCE=.\zend_dynamic_array.c
# End Source File
# Begin Source File

SOURCE=.\zend_execute.c
# End Source File
# Begin Source File

SOURCE=.\zend_execute_API.c
# End Source File
# Begin Source File

SOURCE=.\zend_extensions.c
# End Source File
# Begin Source File

SOURCE=.\zend_float.c
# End Source File
# Begin Source File

SOURCE=.\zend_generators.c
# End Source File
# Begin Source File

SOURCE=.\zend_hash.c
# End Source File
# Begin Source File

SOURCE=.\zend_highlight.c
# End Source File
# Begin Source File

SOURCE=.\zend_indent.c
# End Source File
# Begin Source File

SOURCE=.\zend_ini.c
# End Source File
# Begin Source File

SOURCE=.\zend_ini_parser.c
# End Source File
# Begin Source File

SOURCE=.\zend_ini_scanner.c
# End Source File
# Begin Source File

SOURCE=.\zend_interfaces.c
# End Source File
# Begin Source File

SOURCE=".\zend_language_parser.c"
# End Source File
# Begin Source File

SOURCE=".\zend_language_scanner.c"
# End Source File
# Begin Source File

SOURCE=.\zend_list.c
# End Source File
# Begin Source File

SOURCE=.\zend_llist.c
# End Source File
# Begin Source File

SOURCE=.\zend_object_handlers.c
# End Source File
# Begin Source File

SOURCE=.\zend_objects.c
# End Source File
# Begin Source File

SOURCE=.\zend_objects_API.c
# End Source File
# Begin Source File

SOURCE=.\zend_opcode.c
# End Source File
# Begin Source File

SOURCE=.\zend_operators.c
# End Source File
# Begin Source File

SOURCE=.\zend_ptr_stack.c
# End Source File
# Begin Source File

SOURCE=.\zend_qsort.c
# End Source File
# Begin Source File

SOURCE=.\zend_sprintf.c
# End Source File
# Begin Source File

SOURCE=.\zend_stack.c
# End Source File
# Begin Source File

SOURCE=.\zend_stream.c
# End Source File
# Begin Source File

SOURCE=.\zend_string.c
# End Source File
# Begin Source File

SOURCE=.\zend_strtod.c
# End Source File
# Begin Source File

SOURCE=.\zend_ts_hash.c
# End Source File
# Begin Source File

SOURCE=.\zend_variables.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FlexLexer.h
# End Source File
# Begin Source File

SOURCE=.\zend.h
# End Source File
# Begin Source File

SOURCE=.\zend_alloc.h
# End Source File
# Begin Source File

SOURCE=.\zend_API.h
# End Source File
# Begin Source File

SOURCE=.\zend_builtin_functions.h
# End Source File
# Begin Source File

SOURCE=.\zend_compile.h
# End Source File
# Begin Source File

SOURCE=.\zend_config.w32.h
# End Source File
# Begin Source File

SOURCE=.\zend_constants.h
# End Source File
# Begin Source File

SOURCE=.\zend_default_classes.h
# End Source File
# Begin Source File

SOURCE=.\zend_dynamic_array.h
# End Source File
# Begin Source File

SOURCE=.\zend_errors.h
# End Source File
# Begin Source File

SOURCE=.\zend_execute.h
# End Source File
# Begin Source File

SOURCE=.\zend_execute_locks.h
# End Source File
# Begin Source File

SOURCE=.\zend_extensions.h
# End Source File
# Begin Source File

SOURCE=.\zend_globals.h
# End Source File
# Begin Source File

SOURCE=.\zend_globals_macros.h
# End Source File
# Begin Source File

SOURCE=.\zend_hash.h
# End Source File
# Begin Source File

SOURCE=.\zend_highlight.h
# End Source File
# Begin Source File

SOURCE=.\zend_indent.h
# End Source File
# Begin Source File

SOURCE=.\zend_ini.h
# End Source File
# Begin Source File

SOURCE=.\zend_ini_parser.h
# End Source File
# Begin Source File

SOURCE=.\zend_ini_scanner.h
# End Source File
# Begin Source File

SOURCE=.\zend_interfaces.h
# End Source File
# Begin Source File

SOURCE=.\zend_istdiostream.h
# End Source File
# Begin Source File

SOURCE=".\zend_language_parser.h"
# End Source File
# Begin Source File

SOURCE=".\zend_language_scanner.h"
# End Source File
# Begin Source File

SOURCE=.\zend_list.h
# End Source File
# Begin Source File

SOURCE=.\zend_llist.h
# End Source File
# Begin Source File

SOURCE=.\zend_modules.h
# End Source File
# Begin Source File

SOURCE=.\zend_object_handlers.h
# End Source File
# Begin Source File

SOURCE=.\zend_objects.h
# End Source File
# Begin Source File

SOURCE=.\zend_objects_API.h
# End Source File
# Begin Source File

SOURCE=.\zend_operators.h
# End Source File
# Begin Source File

SOURCE=.\zend_ptr_stack.h
# End Source File
# Begin Source File

SOURCE=.\zend_qsort.h
# End Source File
# Begin Source File

SOURCE=.\zend_stack.h
# End Source File
# Begin Source File

SOURCE=.\zend_stream.h
# End Source File
# Begin Source File

SOURCE=.\zend_string.h
# End Source File
# Begin Source File

SOURCE=.\zend_strtod.h
# End Source File
# Begin Source File

SOURCE=.\zend_ts_hash.h
# End Source File
# Begin Source File

SOURCE=.\zend_variables.h
# End Source File
# Begin Source File

SOURCE=.\zend_virtual_cwd.c
# End Source File
# End Group
# Begin Group "Parsers"

# PROP Default_Filter "y"
# Begin Source File

SOURCE=.\zend_ini_parser.y

!IF  "$(CFG)" == "Zend - Win32 Release"

!ELSEIF  "$(CFG)" == "Zend - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\zend_ini_parser.y

BuildCmds= \
	bison --output=zend_ini_parser.c -v -d -p ini_ zend_ini_parser.y

"$(InputDir)\zend_ini_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\zend_ini_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Release_inline"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\zend_language_parser.y"

!IF  "$(CFG)" == "Zend - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=".\zend_language_parser.y"

BuildCmds= \
	bison --output=zend_language_parser.c -v -d -p zend zend_language_parser.y

"$(InputDir)\zend_language_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\zend_language_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=".\zend_language_parser.y"

BuildCmds= \
	bison --output=zend_language_parser.c -v -d -p zend zend_language_parser.y

"$(InputDir)\zend_language_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\zend_language_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Release_inline"

# Begin Custom Build
InputDir=.
InputPath=".\zend_language_parser.y"

BuildCmds= \
	bison --output=zend_language_parser.c -v -d -p zend zend_language_parser.y

"$(InputDir)\zend_language_parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\zend_language_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Scanners"

# PROP Default_Filter "l"
# Begin Source File

SOURCE=.\flex.skl
# End Source File
# Begin Source File

SOURCE=.\zend_ini_scanner.l

!IF  "$(CFG)" == "Zend - Win32 Release"

!ELSEIF  "$(CFG)" == "Zend - Win32 Debug"

# Begin Custom Build
InputPath=.\zend_ini_scanner.l

"zend_ini_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -B -i -Sflex.skl -Pini_ -ozend_ini_scanner.c zend_ini_scanner.l

# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Release_inline"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\zend_language_scanner.l"

!IF  "$(CFG)" == "Zend - Win32 Release"

# Begin Custom Build
InputPath=".\zend_language_scanner.l"

"zend_language_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -i -Pzend -ozend_language_scanner.c zend_language_scanner.l

# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Debug"

# Begin Custom Build
InputPath=".\zend_language_scanner.l"

"zend_language_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -i -Pzend -ozend_language_scanner.c zend_language_scanner.l

# End Custom Build

!ELSEIF  "$(CFG)" == "Zend - Win32 Release_inline"

# Begin Custom Build
InputPath=".\zend_language_scanner.l"

"zend_language_scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -i -Pzend -ozend_language_scanner.c zend_language_scanner.l

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZEND_BUGS
# End Source File
# Begin Source File

SOURCE=.\ZEND_CHANGES
# End Source File
# Begin Source File

SOURCE=.\ZEND_TODO
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\zend.ico
# End Source File
# End Group
# End Target
# End Project
