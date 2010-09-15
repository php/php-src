# Microsoft Developer Studio Project File - Name="php5dllts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php5dllts - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php5dllts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php5dllts.mak" CFG="php5dllts - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php5dllts - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php5dllts - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php5dllts - Win32 Release_TS_inline" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php5dllts - Win32 Release_TSDbg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\..\zlib" /I "..\Zend" /I "..\TSRM" /I "..\..\libxml\include" /I "..\ext\sqlite\libsqlite\src" /D "_DEBUG" /D ZEND_DEBUG=1 /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /D "LIBXML_THREAD_ENABLED" /D "LIBXML_STATIC" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib zlib.lib libxml2_a.lib Urlmon.lib libsqlite.lib iconv_a.lib /nologo /version:4.0 /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"msvcrt" /out:"..\Debug_TS\php5ts_debug.dll" /pdbtype:sept /libpath:"..\TSRM\Debug_TS" /libpath:"..\Zend\Debug_TS" /libpath:"..\..\bindlib_w32\Debug" /libpath:"Debug_TS" /libpath:"..\..\zlib\Debug" /libpath:"..\..\libxml\lib\Debug" /libpath:"..\ext\sqlite\Debug_TS" /libpath:"..\..\libiconv\lib"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\..\zlib" /I "..\Zend" /I "..\TSRM" /I "..\..\libxml\include" /I "..\ext\sqlite\libsqlite\src" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /D "LIBXML_STATIC" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib zlib.lib libxml2_a.lib Urlmon.lib libsqlite.lib iconv_a.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libcmt" /out:"..\Release_TS\php5ts.dll" /libpath:"..\TSRM\Release_TS" /libpath:"..\Zend\Release_TS" /libpath:"Release_TS" /libpath:"..\ext\sqlite\Release_TS" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\..\zlib\Release" /libpath:"..\..\libxml\lib\Release" /libpath:"..\..\libiconv\lib"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php5dllts___Win32_Release_TS_inline"
# PROP BASE Intermediate_Dir "php5dllts___Win32_Release_TS_inline"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TS_inline"
# PROP Intermediate_Dir "Release_TS_inline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "regex" /I "..\bindlib_w32" /I "Zend" /I "tsrm" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "MSVC5" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "ZTS" /D "WIN32" /D "_MBCS" /D ZEND_DEBUG=0 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\..\zlib" /I "..\Zend" /I "..\TSRM" /I "..\..\libxml\include" /I "..\ext\sqlite\libsqlite\src" /D "NDEBUG" /D ZEND_DEBUG=0 /D "ZEND_WIN32_FORCE_INLINE" /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /D "LIBXML_THREAD_ENABLED" /D "LIBXML_STATIC" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib /nologo /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"Release_TS/php5ts.dll" /libpath:"TSRM\Release_TS" /libpath:"Zend\Release_TS" /libpath:"..\bindlib_w32\Release"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib zlib.lib libxml2_a.lib Urlmon.lib libsqlite.lib iconv_a.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libcmt" /out:"..\Release_TS_inline\php5ts.dll" /libpath:"..\TSRM\Release_TS_inline" /libpath:"..\Zend\Release_TS_inline" /libpath:"Release_TS_Inline" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\..\zlib\Release" /libpath:"..\..\libxml\lib\Release" /libpath:"..\..\libiconv\lib" /libpath:"..\ext\sqlite\Release_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "php5dllts___Win32_Release_TSDbg"
# PROP BASE Intermediate_Dir "php5dllts___Win32_Release_TSDbg"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_TSDbg"
# PROP Intermediate_Dir "Release_TSDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\Zend" /I "..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I ".." /I "..\main" /I "..\regex" /I "..\..\bindlib_w32" /I "..\..\zlib" /I "..\Zend" /I "..\TSRM" /I "..\..\libxml\include" /I "..\ext\sqlite\libsqlite\src" /D "NDEBUG" /D ZEND_DEBUG=0 /D "_WINDOWS" /D "_USRDLL" /D "PHP5DLLTS_EXPORTS" /D "PHP_EXPORTS" /D "LIBZEND_EXPORTS" /D "TSRM_EXPORTS" /D "SAPI_EXPORTS" /D "MSVC5" /D "ZTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "_MBCS" /D "LIBXML_THREAD_ENABLED" /D "LIBXML_STATIC" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib /nologo /version:4.0 /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"..\Release_TS\php5ts.dll" /libpath:"..\TSRM\Release_TS" /libpath:"..\Zend\Release_TS" /libpath:"..\..\bindlib_w32\Release" /libpath:"Release_TS"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ZendTS.lib TSRM.lib resolv.lib zlib.lib libxml2_a.lib Urlmon.lib libsqlite.lib iconv_a.lib /nologo /version:4.0 /dll /debug /machine:I386 /nodefaultlib:"libcmt" /out:"..\Release_TSDbg\php5ts.dll" /libpath:"..\TSRM\Release_TSDbg" /libpath:"..\Zend\Release_TSDbg" /libpath:"Release_TSDbg" /libpath:"..\ext\sqlite\Release_TSDbg" /libpath:"..\..\bindlib_w32\Release" /libpath:"..\..\zlib\Release" /libpath:"..\..\libxml\lib\Release" /libpath:"..\..\libiconv\lib"

!ENDIF 

# Begin Target

# Name "php5dllts - Win32 Debug_TS"
# Name "php5dllts - Win32 Release_TS"
# Name "php5dllts - Win32 Release_TS_inline"
# Name "php5dllts - Win32 Release_TSDbg"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\standard\css.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\cyr_convert.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\filters.c
# End Source File
# Begin Source File

SOURCE="..\main\fopen_wrappers.c"
# End Source File
# Begin Source File

SOURCE=.\globals.c
# End Source File
# Begin Source File

SOURCE=..\main\internal_functions_win32.c
# End Source File
# Begin Source File

SOURCE=..\main\main.c
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

SOURCE=..\main\php_scandir.c
# End Source File
# Begin Source File

SOURCE=..\main\php_sprintf.c
# End Source File
# Begin Source File

SOURCE=..\main\php_ticks.c
# End Source File
# Begin Source File

SOURCE=..\main\php_variables.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\proc_open.c
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

SOURCE=..\main\strlcat.c
# End Source File
# Begin Source File

SOURCE=..\main\strlcpy.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\main\config.w32.h
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

SOURCE=..\main\logos.h
# End Source File
# Begin Source File

SOURCE=..\main\output.h
# End Source File
# Begin Source File

SOURCE=..\main\php.h
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

SOURCE=..\main\php_scandir.h
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

SOURCE=..\ext\pcre\pcrelib\pcre_chartables.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_compile.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_exec.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_fullinfo.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_get.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_globals.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_info.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_maketables.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_ord2utf8.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_study.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_tables.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_try_flipped.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_ucp_searchfuncs.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_valid_utf8.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_version.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
# End Source File
# Begin Source File

SOURCE=..\ext\pcre\pcrelib\pcre_xclass.c
# ADD CPP /D "SUPPORT_UTF8" /D LINK_SIZE=2 /D MATCH_LIMIT=10000000 /D MATCH_LIMIT_RECURSION=10000000 /D NEWLINE=10 /D "SUPPORT_UCP" /D MAX_NAME_SIZE=32 /D MAX_NAME_COUNT=10000 /D MAX_DUPLENGTH=30000 /D "NO_RECURSE"
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
# Begin Group "DOM"

# PROP Default_Filter ""
# Begin Group "DOM Source Files"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\dom\attr.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\cdatasection.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\characterdata.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\comment.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\document.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\documentfragment.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\documenttype.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\dom_iterators.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domconfiguration.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domerror.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domerrorhandler.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domexception.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domimplementation.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domimplementationlist.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domimplementationsource.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domlocator.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\domstringlist.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\element.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\entity.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\entityreference.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\namednodemap.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\namelist.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\node.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\nodelist.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\notation.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\php_dom.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\processinginstruction.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\string_extend.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\text.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\typeinfo.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\userdatahandler.c
# End Source File
# Begin Source File

SOURCE=..\ext\dom\xpath.c
# End Source File
# End Group
# Begin Group "DOM Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\dom\dom_ce.h
# End Source File
# Begin Source File

SOURCE=..\ext\dom\dom_fe.h
# End Source File
# Begin Source File

SOURCE=..\ext\dom\dom_properties.h
# End Source File
# Begin Source File

SOURCE=..\ext\dom\php_dom.h
# End Source File
# Begin Source File

SOURCE=..\ext\dom\xml_common.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Group
# Begin Group "SimpleXML"

# PROP Default_Filter ""
# Begin Group "SimpleXML Source Files"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\simplexml\simplexml.c
# End Source File
# End Group
# Begin Group "SimpleXML Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\ext\simplexml\php_simplexml.h
# End Source File
# End Group
# Begin Group "Resource Files No. 1"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
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

SOURCE=..\ext\xml\compat.c
# End Source File
# Begin Source File

SOURCE=..\ext\ctype\ctype.c
# End Source File
# Begin Source File

SOURCE=..\ext\tokenizer\tokenizer.c
# End Source File
# Begin Source File

SOURCE=..\ext\wddx\wddx.c
# End Source File
# Begin Source File

SOURCE=..\ext\xml\xml.c
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
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\calendar.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\dow.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\easter.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\french.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\gregor.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\jewish.c
# PROP Intermediate_Dir "calendar"
# End Source File
# Begin Source File

SOURCE=..\ext\calendar\julian.c
# PROP Intermediate_Dir "calendar"
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
# Begin Group "SPL"

# PROP Default_Filter ""
# Begin Group "Source Files No. 11"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\spl\php_spl.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_array.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_directory.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_engine.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_exceptions.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_functions.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_iterators.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_observer.c
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_sxe.c
# End Source File
# End Group
# Begin Group "Header Files No. 12"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\spl\php_spl.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_array.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_directory.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_engine.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_exceptions.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_functions.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_iterators.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_observer.h
# End Source File
# Begin Source File

SOURCE=..\ext\spl\spl_sxe.h
# End Source File
# End Group
# End Group
# Begin Group "Reflection"

# PROP Default_Filter ""
# Begin Group "Source Files No. 12"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\reflection\php_reflection.c
# End Source File
# End Group
# Begin Group "Header Files No. 13"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\reflection\php_reflection.h
# End Source File
# End Group
# End Group
# Begin Group "XMLReader"

# PROP Default_Filter ""
# Begin Group "Source Files No. 13"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\xmlreader\php_xmlreader.c
# End Source File
# End Group
# Begin Group "Header Files No. 14"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\xmlreader\php_xmlreader.h
# End Source File
# End Group
# End Group
# Begin Group "XMLwriter"

# PROP Default_Filter ""
# Begin Group "Source Files No. 14"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\xmlwriter\php_xmlwriter.c
# End Source File
# End Group
# Begin Group "Header Files No. 15"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\xmlwriter\php_xmlwriter.h
# End Source File
# End Group
# End Group
# Begin Group "IConv"

# PROP Default_Filter ""
# Begin Group "Source Files No. 15"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\iconv\iconv.c
# ADD CPP /D "PHP_ICONV_EXPORTS"
# End Source File
# End Group
# Begin Group "Header Files No. 16"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\iconv\php_iconv.h
# End Source File
# End Group
# End Group
# Begin Group "bcmath"

# PROP Default_Filter ""
# Begin Group "Source Files No. 8"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\add.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\compare.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\debug.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\div.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\divmod.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\doaddsub.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\init.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\int2num.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\nearzero.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\neg.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2long.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\num2str.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\outofmem.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\output.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raise.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\raisemod.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\recmul.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rmzero.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\rt.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sqrt.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\str2num.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\sub.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD BASE CPP /I "..\ext\bcmath\libbcmath\src"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\zero.c

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"
# ADD CPP /I "..\ext\bcmath\libbcmath\src"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

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

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\config.h

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\bcmath\libbcmath\src\private.h

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Debug_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# PROP BASE Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"
# PROP Intermediate_Dir "ext\bcmath\libbcmath\Release_TS"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Group "Source Files No. 10 Nr. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\zlib\zlib.c
# End Source File
# Begin Source File

SOURCE=..\ext\zlib\zlib_filter.c
# End Source File
# Begin Source File

SOURCE=..\ext\zlib\zlib_fopen_wrapper.c
# End Source File
# End Group
# Begin Group "Header Files No. 10 Nr. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\zlib\php_zlib.h
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

SOURCE=..\ext\standard\http.c
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

SOURCE=..\ext\standard\php_fopen_wrapper.c
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

SOURCE=..\ext\standard\streamsfuncs.c
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

SOURCE=..\ext\standard\url_scanner_ex.c
# ADD CPP /W2
# End Source File
# Begin Source File

SOURCE=..\ext\standard\uuencode.c
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

SOURCE=..\ext\standard\php_http.h
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

SOURCE=..\ext\standard\php_sunfuncs.h
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

SOURCE=..\ext\standard\url_scanner_ex.h
# End Source File
# End Group
# End Group
# Begin Group "SQLite"

# PROP Default_Filter ""
# Begin Group "Header Files No. 5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\sqlite\php_sqlite.h
# End Source File
# End Group
# Begin Group "Source Files No. 5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\sqlite\sess_sqlite.c
# End Source File
# Begin Source File

SOURCE=..\ext\sqlite\sqlite.c
# End Source File
# End Group
# End Group
# Begin Group "LIBXML"

# PROP Default_Filter ""
# Begin Group "Header Files No. 10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\libxml\php_libxml.h
# End Source File
# End Group
# Begin Group "Source Files No. 9"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\libxml\libxml.c
# End Source File
# End Group
# End Group
# Begin Group "Date"

# PROP Default_Filter ""
# Begin Group "Source Files No. 10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\date\lib\astro.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\dow.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\parse_date.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\parse_tz.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\php_date.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\timelib.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\timelib_config.h.win32

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

# Begin Custom Build
InputDir=\Projects\php-5.2\ext\date\lib
InputPath=..\ext\date\lib\timelib_config.h.win32

"..\ext\date\lib\timelib_config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\timelib_config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

# Begin Custom Build
InputDir=\Projects\php-5.2\ext\date\lib
InputPath=..\ext\date\lib\timelib_config.h.win32

"..\ext\date\lib\timelib_config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\timelib_config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

# Begin Custom Build
InputDir=\Projects\php-5.2\ext\date\lib
InputPath=..\ext\date\lib\timelib_config.h.win32

"..\ext\date\lib\timelib_config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\timelib_config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# Begin Custom Build
InputDir=\Projects\php-5.2\ext\date\lib
InputPath=..\ext\date\lib\timelib_config.h.win32

"..\ext\date\lib\timelib_config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(InputDir)\timelib_config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\tm2unixtime.c
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\unixtime2tm.c
# End Source File
# End Group
# Begin Group "Header Files No. 11"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\date\lib\astro.h
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\fallbackmap.h
# End Source File
# Begin Source File

SOURCE=..\ext\date\php_date.h
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\timelib.h
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\timezonedb.h
# End Source File
# Begin Source File

SOURCE=..\ext\date\lib\timezonemap.h
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

SOURCE=..\win32\readdir.c
# End Source File
# Begin Source File

SOURCE=..\win32\registry.c
# End Source File
# Begin Source File

SOURCE=.\select.c
# End Source File
# Begin Source File

SOURCE=..\win32\sendmail.c
# End Source File
# Begin Source File

SOURCE=..\win32\time.c
# End Source File
# Begin Source File

SOURCE=..\win32\winutil.c
# End Source File
# Begin Source File

SOURCE=..\win32\wsyslog.c
# End Source File
# Begin Source File

SOURCE=.\build\wsyslog.mc

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

# Begin Custom Build
InputDir=.\build
IntDir=.\Release_TSDbg
InputPath=.\build\wsyslog.mc

"wsyslog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mc -h $(InputDir)/.. -r $(InputDir) -x $(IntDir) $(InputPath)

# End Custom Build

!ENDIF 

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

SOURCE=..\win32\readdir.h
# End Source File
# Begin Source File

SOURCE=.\select.h
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
# End Group
# Begin Group "Streams"

# PROP Default_Filter ""
# Begin Group "streams headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\main\streams\php_stream_context.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_stream_filter_api.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_stream_mmap.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_stream_plain_wrapper.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_stream_transport.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_stream_userspace.h
# End Source File
# Begin Source File

SOURCE=..\main\streams\php_streams_int.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\main\streams\cast.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\filter.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\memory.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\mmap.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\plain_wrapper.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\streams.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\transports.c
# End Source File
# Begin Source File

SOURCE=..\ext\standard\user_filters.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\userspace.c
# End Source File
# Begin Source File

SOURCE=..\main\streams\xp_socket.c
# End Source File
# End Group
# Begin Group "COM and DotNet"

# PROP Default_Filter ""
# Begin Group "Header Files No. 9"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\com_dotnet\com_saproxy.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_wrapper.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\php_com_dotnet.h
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\php_com_dotnet_internal.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ext\com_dotnet\com_com.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_dotnet.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_extension.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_handlers.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_iterator.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_misc.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_olechar.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_persist.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_typeinfo.c
# End Source File
# Begin Source File

SOURCE=..\ext\com_dotnet\com_variant.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\php5dllts.rc
# End Source File
# Begin Source File

SOURCE=.\php5dllts.rc2
# End Source File
# Begin Source File

SOURCE=.\phpts.def

!IF  "$(CFG)" == "php5dllts - Win32 Debug_TS"

USERDEP__PHPTS="..\ext\sqlite\php_sqlite.def"	"..\ext\libxml\php_libxml2.def"	
# Begin Custom Build - Generating $(InputPath)
InputPath=.\phpts.def

"phpts.def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	builddef.bat > phpts.def

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS"

USERDEP__PHPTS="..\ext\sqlite\php_sqlite.def"	"..\ext\libxml\php_libxml2.def"	
# Begin Custom Build - Generating $(InputPath)
InputPath=.\phpts.def

"phpts.def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	builddef.bat > phpts.def

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TS_inline"

USERDEP__PHPTS="..\ext\sqlite\php_sqlite.def"	"..\ext\libxml\php_libxml2.def"	
# Begin Custom Build - Generating $(InputPath)
InputPath=.\phpts.def

"phpts.def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	builddef.bat > phpts.def

# End Custom Build

!ELSEIF  "$(CFG)" == "php5dllts - Win32 Release_TSDbg"

USERDEP__PHPTS="..\ext\sqlite\php_sqlite.def"	"..\ext\libxml\php_libxml2.def"	
# Begin Custom Build - Generating $(InputPath)
InputPath=.\phpts.def

"phpts.def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	builddef.bat > phpts.def

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
