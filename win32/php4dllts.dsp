# Microsoft Developer Studio Project File - Name="php4dllts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php4dllts - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php4dllts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php4dllts.mak" CFG="php4dllts - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php4dllts - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php4dllts - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php4dllts - Win32 Release_TS_inline" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php4dllts - Win32 Release_TSDbg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /I "..\ext\mysql\libmysql" /I "..\ext\xml\expat" /D "_DEBUG" /D ZEND_DEBUG=1 /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib libmysql.lib /nologo /version:4.0 /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"..\Debug_TS\php4ts_debug.dll" /pdbtype:sept /libpath:"..\TSRM\Debug_TS" /libpath:"..\Zend\Debug_TS" /libpath:"..\..\bindlib_w32\Debug" /libpath:"..\ext\mysql\libmysql\Debug_TS" /libpath:"Debug_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /I "..\ext\mysql\libmysql" /I "..\ext\xml\expat" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib libmysql.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"..\Release_TS\php4ts.dll" /libpath:"..\TSRM\Release_TS" /libpath:"..\Zend\Release_TS" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\ext\mysql\libmysql\Release_TS" /libpath:"Release_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php4dllts___Win32_Release_TS_inline"
# PROP BASE Intermediate_Dir "php4dllts___Win32_Release_TS_inline"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "regex" /I "..\bindlib_w32" /I "Zend" /I "tsrm" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "MSVC5" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "ZTS" /D "WIN32" /D "_MBCS" /D ZEND_DEBUG=0 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /I "..\ext\mysql\libmysql" /I "..\ext\xml\expat" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZEND_WIN32_FORCE_INLINE" /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib /nologo /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"Release_TS/php4ts.dll" /libpath:"TSRM\Release_TS" /libpath:"Zend\Release_TS" /libpath:"..\bindlib_w32\Release"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib libmysql.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"..\Release_TS_inline\php4ts.dll" /libpath:"..\TSRM\Release_TS_inline" /libpath:"..\Zend\Release_TS_inline" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\ext\mysql\libmysql\Release_TS_inline" /libpath:"Release_TS_Inline"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php4dllts___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "php4dllts___Win32_Release_TSDbg"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TSDbg"
# PROP Intermediate_Dir "Release_TSDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /I "..\ext\mysql\libmysql" /I "..\ext\xml\expat" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /I "..\ext\mysql\libmysql" /I "..\ext\xml\expat" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP4DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib libmysql.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"..\Release_TS\php4ts.dll" /libpath:"..\TSRM\Release_TS" /libpath:"..\Zend\Release_TS" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\ext\mysql\libmysql\Release_TS" /libpath:"Release_TS"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib libmysql.lib /nologo /version:4.0 /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"..\Release_TSDbg\php4ts.dll" /libpath:"..\TSRM\Release_TSDbg" /libpath:"..\Zend\Release_TSDbg" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\ext\mysql\libmysql\Release_TSDbg" /libpath:"Release_TSDbg"

!ENDIF 

# Begin Target

# Name "php4dllts - Win32 Debug_TS"
# Name "php4dllts - Win32 Release_TS"
# Name "php4dllts - Win32 Release_TS_inline"
# Name "php4dllts - Win32 Release_TSDbg"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\aggregation.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\css.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\cyr_convert.c
# End Source File
# Begin Source File

SOURCE="..\main\fopen_wrappers.c"
# End Source File
# Begin Source File

SOURCE=..\main\internal_functions_win32.c
# ADD CPP /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"
# End Source File
# Begin Source File

SOURCE=..\main\main.c
# End Source File
# Begin Source File

SOURCE=..\main\memory_streams.c
# End Source File
# Begin Source File

SOURCE=..\main\mergesort.c
# End Source File
# Begin Source File

SOURCE=..\main\network.c
# End Source File
# Begin Source File

SOURCE=..\main\output.c
# End Source File
# Begin Source File

SOURCE=..\main\php_content_types.c
# End Source File
# Begin Source File

SOURCE=..\main\php_ini.c
# End Source File
# Begin Source File

SOURCE=..\main\php_logos.c
# End Source File
# Begin Source File

SOURCE=..\main\php_open_temporary_file.c
# End Source File
# Begin Source File

SOURCE=..\main\php_ticks.c
# End Source File
# Begin Source File

SOURCE=..\main\php_variables.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\quot_print.c
# End Source File
# Begin Source File

SOURCE=..\main\reentrancy.c
# End Source File
# Begin Source File

SOURCE=..\main\rfc1867.c
# End Source File
# Begin Source File

SOURCE=..\main\safe_mode.c
# End Source File
# Begin Source File

SOURCE=..\main\SAPI.c
# End Source File
# Begin Source File

SOURCE=..\main\snprintf.c
# End Source File
# Begin Source File

SOURCE=..\main\spprintf.c
# End Source File
# Begin Source File

SOURCE=..\main\streams.c
# End Source File
# Begin Source File

SOURCE=..\main\strlcat.c
# End Source File
# Begin Source File

SOURCE=..\main\strlcpy.c
# End Source File
# Begin Source File

SOURCE=..\main\user_streams.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\aggregation.h
# End Source File
# Begin Source File

SOURCE=..\main\config.w32.h.in

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# Begin Custom Build - Generating main/config.w32.h
InputPath=..\main\config.w32.h.in

"..\main\config.w32.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\main\config.w32.h ( 
	copy ..\main\config.w32.h.in ..\main\config.w32.h > nul 
	) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# Begin Custom Build - Generating main/config.w32.h
InputPath=..\main\config.w32.h.in

"..\main\config.w32.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\main\config.w32.h ( 
	copy ..\main\config.w32.h.in ..\main\config.w32.h > nul 
	) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# Begin Custom Build - Generating main/config.w32.h
InputPath=..\main\config.w32.h.in

"..\main\config.w32.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\main\config.w32.h ( 
	copy ..\main\config.w32.h.in ..\main\config.w32.h > nul 
	) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# Begin Custom Build - Generating main/config.w32.h
InputPath=..\main\config.w32.h.in

"..\main\config.w32.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\main\config.w32.h ( 
	copy ..\main\config.w32.h.in ..\main\config.w32.h > nul 
	) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\standard\css.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\cyr_convert.h
# End Source File
# Begin Source File

SOURCE="..\main\fopen_wrappers.h"
# End Source File
# Begin Source File

SOURCE=..\main\getopt.h
# End Source File
# Begin Source File

SOURCE=..\main\internal_functions_registry.h
# End Source File
# Begin Source File

SOURCE=..\main\logos.h
# End Source File
# Begin Source File

SOURCE=..\main\output.h
# End Source File
# Begin Source File

SOURCE=..\main\php.h
# End Source File
# Begin Source File

SOURCE=..\main\php3_compat.h
# End Source File
# Begin Source File

SOURCE=..\main\php_compat.h
# End Source File
# Begin Source File

SOURCE=..\main\php_content_types.h
# End Source File
# Begin Source File

SOURCE=..\main\php_globals.h
# End Source File
# Begin Source File

SOURCE=..\main\php_ini.h
# End Source File
# Begin Source File

SOURCE=..\main\php_logos.h
# End Source File
# Begin Source File

SOURCE=..\main\php_main.h
# End Source File
# Begin Source File

SOURCE=..\main\php_open_temporary_file.h
# End Source File
# Begin Source File

SOURCE=..\main\php_output.h
# End Source File
# Begin Source File

SOURCE=..\main\php_regex.h
# End Source File
# Begin Source File

SOURCE=..\main\php_streams.h
# End Source File
# Begin Source File

SOURCE=..\main\php_variables.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\quot_print.h
# End Source File
# Begin Source File

SOURCE=..\main\rfc1867.h
# End Source File
# Begin Source File

SOURCE=..\main\safe_mode.h
# End Source File
# Begin Source File

SOURCE=..\main\SAPI.h
# End Source File
# Begin Source File

SOURCE=..\main\snprintf.h
# End Source File
# Begin Source File

SOURCE=..\main\spprintf.h
# End Source File
# Begin Source File

SOURCE=..\main\win95nt.h
# End Source File
# End Group
# End Group
# Begin Group "Function Modules"

# PROP Default_Filter ""
# Begin Group "PCRE"

# PROP Default_Filter ""
# Begin Group "Source Files No. 3"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\chartables.c
# ADD CPP /D "STATIC" /D "SUPPORT_UTF8"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\get.c
# ADD CPP /D "STATIC" /D "SUPPORT_UTF8" /D LINK_SIZE=2
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\maketables.c
# ADD CPP /D "STATIC" /D "SUPPORT_UTF8" /D LINK_SIZE=2
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre.c
# ADD CPP /D "STATIC" /D "SUPPORT_UTF8" /D LINK_SIZE=2
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\study.c
# ADD CPP /D "STATIC" /D "SUPPORT_UTF8" /D LINK_SIZE=2
# End Source File
# End Group
# Begin Group "Header Files No. 3"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\internal.h
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre.h
# End Source File
# End Group
# End Group
# Begin Group "Regular Expressions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=..\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=..\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=..\regex\regfree.c
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter ""
# Begin Group "Source Files No. 4"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\ctype\ctype.c
# End Source File
# Begin Source File

SOURCE=..\ext\overload\overload.c
# End Source File
# Begin Source File

SOURCE=..\ext\tokenizer\tokenizer.c
# End Source File
# Begin Source File

SOURCE=..\ext\wddx\wddx.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# ADD CPP /I "..\ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# ADD BASE CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"
# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\xml\xml.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# ADD CPP /I "..\ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# ADD BASE CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"
# ADD CPP /I "ext\xml\expat\xmlparse" /I "..\ext\xml\expat\xmltok" /I "..\ext\xml\expat\xmlparse"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files No. 4"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\wddx\php_wddx.h
# End Source File
# Begin Source File

SOURCE=..\ext\wddx\php_wddx_api.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\php_xml.h
# End Source File
# End Group
# End Group
# Begin Group "FTP"

# PROP Default_Filter ""
# Begin Group "Source Files No. 6"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\ftp\ftp.c
# End Source File
# Begin Source File

SOURCE=..\ext\ftp\php_ftp.c
# End Source File
# End Group
# Begin Group "Header Files No. 6"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\ftp\ftp.h
# End Source File
# Begin Source File

SOURCE=..\ext\ftp\php_ftp.h
# End Source File
# End Group
# End Group
# Begin Group "Calendar"

# PROP Default_Filter ""
# Begin Group "Source Files No. 7"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\calendar\cal_unix.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\calendar.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\dow.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\easter.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\french.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\gregor.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\jewish.c
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\julian.c
# End Source File
# End Group
# Begin Group "Header Files No. 7"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\calendar\php_calendar.h
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\sdncal.h
# End Source File
# End Group
# End Group
# Begin Group "bcmath"

# PROP Default_Filter ""
# Begin Group "Source Files No. 8"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\add.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\compare.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\debug.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\div.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\divmod.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\doaddsub.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\init.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\int2num.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\nearzero.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\neg.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2long.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2str.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\outofmem.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\output.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raise.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raisemod.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\recmul.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rmzero.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rt.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sqrt.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\str2num.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sub.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\zero.c

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files No. 8"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\bcmath.h

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\config.h

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\private.h

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "COM"

# PROP Default_Filter ""
# Begin Group "Source Files No. 9"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\com\COM.c
# End Source File
# Begin Source File

SOURCE=..\ext\com\conversion.c
# End Source File
# Begin Source File

SOURCE=..\ext\com\dispatch.c
# End Source File
# Begin Source File

SOURCE=..\ext\com\VARIANT.c
# End Source File
# End Group
# Begin Group "Header Files No. 9"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\com\com.h
# End Source File
# Begin Source File

SOURCE=..\ext\com\conversion.h
# End Source File
# Begin Source File

SOURCE=..\ext\com\php_COM.h
# End Source File
# Begin Source File

SOURCE=..\ext\com\php_VARIANT.h
# End Source File
# Begin Source File

SOURCE=..\ext\com\variant.h
# End Source File
# End Group
# End Group
# Begin Group "Standard"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\array.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\assert.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\base64.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\basic_functions.c
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\bcmath.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\browscap.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\crc32.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\credits.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\crypt.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\datetime.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\dir.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\dl.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\dns.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\exec.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\file.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\filestat.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\flock_compat.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\formatted_print.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\fsock.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\ftp_fopen_wrapper.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\head.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\html.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\http_fopen_wrapper.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\image.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\incomplete_class.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\info.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\iptc.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\lcg.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\levenshtein.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\link.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\mail.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\math.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\md5.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\metaphone.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\microtime.c
# End Source File
# Begin Source File

SOURCE=..\ext\session\mod_files.c
# End Source File
# Begin Source File

SOURCE=..\ext\session\mod_user.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\pack.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\pageinfo.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\parsedate.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_fopen_wrapper.c
# End Source File
# Begin Source File

SOURCE=..\ext\mcal\php_mcal.c
# End Source File
# Begin Source File

SOURCE=..\ext\mysql\php_mysql.c
# End Source File
# Begin Source File

SOURCE=..\ext\odbc\php_odbc.c
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\php_pcre.c
# ADD CPP /D "STATIC"
# End Source File
# Begin Source File

SOURCE=..\ext\standard\rand.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\reg.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\scanf.c
# End Source File
# Begin Source File

SOURCE=..\ext\session\session.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\sha1.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\soundex.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\string.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\strnatcmp.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\syslog.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\type.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\uniqid.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url_scanner.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url_scanner_ex.c
# ADD CPP /W2
# End Source File
# Begin Source File

SOURCE=..\ext\standard\var.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\var_unserializer.c
# ADD CPP /W2
# End Source File
# Begin Source File

SOURCE=..\ext\standard\versioning.c
# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\base64.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\basic_functions.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\datetime.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\dl.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\dns.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\exec.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\file.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\flock_compat.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\fsock.h
# End Source File
# Begin Source File

SOURCE=..\functions\global.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\head.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\html.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\info.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\md5.h
# End Source File
# Begin Source File

SOURCE=..\ext\session\mod_user.h
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\number.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\pageinfo.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_array.h
# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\php_bcmath.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_crypt.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_dir.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_filestat.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_fopen_wrappers.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_lcg.h
# End Source File
# Begin Source File

SOURCE=..\ext\ldap\php_ldap.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_mail.h
# End Source File
# Begin Source File

SOURCE=..\ext\mcal\php_mcal.h
# End Source File
# Begin Source File

SOURCE=..\ext\mysql\php_mysql.h
# End Source File
# Begin Source File

SOURCE=..\ext\odbc\php_odbc.h
# End Source File
# Begin Source File

SOURCE=..\ext\odbc\php_odbc_includes.h
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\php_pcre.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_rand.h
# End Source File
# Begin Source File

SOURCE=..\ext\session\php_session.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_string.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\php_syslog.h
# End Source File
# Begin Source File

SOURCE=..\functions\phpdir.h
# End Source File
# Begin Source File

SOURCE=..\functions\phpmath.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\reg.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\scanf.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\sha1.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\type.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\uniqid.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url_scanner.h
# End Source File
# Begin Source File

SOURCE=..\ext\standard\url_scanner_ex.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Group "Source Files No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\crypt_win32.c
# End Source File
# Begin Source File

SOURCE=.\glob.c
# End Source File
# Begin Source File

SOURCE=.\md5crypt.c
# End Source File
# Begin Source File

SOURCE=..\win32\pwd.c
# End Source File
# Begin Source File

SOURCE=..\win32\readdir.c
# End Source File
# Begin Source File

SOURCE=..\win32\registry.c
# End Source File
# Begin Source File

SOURCE=..\win32\sendmail.c
# End Source File
# Begin Source File

SOURCE=..\win32\time.c
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.c
# End Source File
# Begin Source File

SOURCE=..\ext\snmp\winsnmp.c
# End Source File
# Begin Source File

SOURCE=..\win32\winutil.c
# End Source File
# Begin Source File

SOURCE=..\win32\wsyslog.c
# End Source File
# End Group
# Begin Group "Header Files No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\crypt_win32.h
# End Source File
# Begin Source File

SOURCE=.\glob.h
# End Source File
# Begin Source File

SOURCE=..\win32\grp.h
# End Source File
# Begin Source File

SOURCE=..\win32\param.h
# End Source File
# Begin Source File

SOURCE=..\win32\php_registry.h
# End Source File
# Begin Source File

SOURCE=..\win32\pwd.h
# End Source File
# Begin Source File

SOURCE=..\win32\readdir.h
# End Source File
# Begin Source File

SOURCE=..\win32\sendmail.h
# End Source File
# Begin Source File

SOURCE=..\win32\syslog.h
# End Source File
# Begin Source File

SOURCE=..\win32\time.h
# End Source File
# Begin Source File

SOURCE=..\win32\unistd.h
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.h
# End Source File
# Begin Source File

SOURCE=.\winutil.h
# End Source File
# End Group
# End Group
# Begin Group "Parsers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\parsedate.y

!IF  "$(CFG)" == "php4dllts - Win32 Debug_TS"

# Begin Custom Build - Generating ext/standard/parsedate.c
InputPath=..\ext\standard\parsedate.y

"..\ext\standard\parsedate.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\ext\standard 
	bison --output=parsedate.c -v -d parsedate.y 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS"

# Begin Custom Build - Generating ext/standard/parsedate.c
InputPath=..\ext\standard\parsedate.y

"..\ext\standard\parsedate.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\ext\standard 
	bison --output=parsedate.c -v -d parsedate.y 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TS_inline"

# Begin Custom Build - Generating ext/standard/parsedate.c
InputPath=..\ext\standard\parsedate.y

"..\ext\standard\parsedate.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\ext\standard 
	bison --output=parsedate.c -v -d parsedate.y 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php4dllts - Win32 Release_TSDbg"

# Begin Custom Build - Generating ext/standard/parsedate.c
InputPath=..\ext\standard\parsedate.y

"..\ext\standard\parsedate.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\ext\standard 
	bison --output=parsedate.c -v -d parsedate.y 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\LICENSE
# End Source File
# Begin Source File

SOURCE=..\NEWS
# End Source File
# Begin Source File

SOURCE="..\php.ini-dist"
# End Source File
# Begin Source File

SOURCE="..\php.ini-recommended"
# End Source File
# Begin Source File

SOURCE="..\README.CVS-RULES"
# End Source File
# Begin Source File

SOURCE=..\TODO
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Group "Expat"

# PROP Default_Filter ""
# Begin Group "Source Files No. 5"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\xml\expat\xmlparse.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\xmlrole.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\xmltok.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# End Group
# Begin Group "Header Files No. 5"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\xml\expat\ascii.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\asciitab.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\expat.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\iasciitab.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\latin1tab.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\nametab.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\utf8tab.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\winconfig.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\xmlrole.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\xmltok.h
# End Source File
# Begin Source File

SOURCE=..\ext\xml\expat\xmltok_impl.h
# End Source File
# End Group
# End Group
# End Group
# Begin Source File

SOURCE=.\php4dllts.rc
# End Source File
# Begin Source File

SOURCE=.\php4dllts.rc2
# End Source File
# End Target
# End Project
