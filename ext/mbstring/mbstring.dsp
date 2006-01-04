# Microsoft Developer Studio Project File - Name="mbstring" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mbstring - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mbstring.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mbstring.mak" CFG="mbstring - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mbstring - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mbstring - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mbstring - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mbstring___Win32_Release_TS"
# PROP BASE Intermediate_Dir "mbstring___Win32_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MBSTRING_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "libmbfl" /I "libmbfl\mbfl" /I "mbregex" /D ZEND_DEBUG=0 /D "_MBCS" /D "_USRDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_MBSTRING" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D MBSTRING_EXPORTS=1 /D HAVE_MBSTRING=1 /D HAVE_MBREGEX=1 /D HAVE_CONFIG_H=1 /D MBFL_DLL_EXPORT=1 /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_mbstring.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "mbstring - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mbstring___Win32_Debug_TS"
# PROP BASE Intermediate_Dir "mbstring___Win32_Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mbstring___Win32_Debug_TS"
# PROP Intermediate_Dir "mbstring___Win32_Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MBSTRING_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "libmbfl" /I "libmbfl\mbfl" /I "mbregex" /D ZEND_DEBUG=1 /D "MBSTRING_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_MBSTRING" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D MBSTRING_EXPORTS=1 /D HAVE_MBSTRING=1 /D HAVE_MBREGEX=1 /D HAVE_CONFIG_H=1 /D MBFL_DLL_EXPORT=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_mbstring.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ENDIF 

# Begin Target

# Name "mbstring - Win32 Release_TS"
# Name "mbstring - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\mbregex\mbregex.c
# End Source File
# Begin Source File

SOURCE=.\mbstring.c
# End Source File
# Begin Source File

SOURCE=.\php_mbregex.c
# End Source File
# Begin Source File

SOURCE=.\php_unicode.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mbregex\mbregex.h
# End Source File
# Begin Source File

SOURCE=.\mbstring.h
# End Source File
# Begin Source File

SOURCE=.\php_unicode.h
# End Source File
# Begin Source File

SOURCE=.\unicode_data.h
# End Source File
# End Group
# Begin Group "libmbfl"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libmbfl\filters\html_entities.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_7bit.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_8bit.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ascii.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_base64.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_big5.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_byte2.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_byte4.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp1251.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp1252.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp866.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp932.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp936.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_cn.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_jp.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_jp_win.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_kr.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_tw.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_htmlent.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_hz.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso2022_kr.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_1.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_10.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_13.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_14.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_15.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_16.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_2.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_3.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_4.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_5.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_6.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_7.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_8.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_9.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_jis.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_koi8r.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_pass.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_armscii8.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_qprint.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_sjis.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ucs2.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ucs4.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_uhc.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf16.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf32.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf7.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf7imap.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf8.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_uuencode.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_wchar.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_allocators.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_convert.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_encoding.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_filter_output.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_ident.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_language.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_memory_device.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_string.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_de.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_en.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_ja.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_kr.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_neutral.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_ru.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_uni.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_zh.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_hy.c
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_tr.c
# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libmbfl\config.h.vc6

!IF  "$(CFG)" == "mbstring - Win32 Release_TS"

# Begin Custom Build
InputDir=.\libmbfl
InputPath=.\libmbfl\config.h.vc6

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputDir)\config.h.vc6 "$(InputDir)\config.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "mbstring - Win32 Debug_TS"

# Begin Custom Build
InputDir=.\libmbfl
InputPath=.\libmbfl\config.h.vc6

"$(InputDir)\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputDir)\config.h.vc6 "$(InputDir)\config.h"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\cp932_table.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\html_entities.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_7bit.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_8bit.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ascii.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_base64.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_big5.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_byte2.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_byte4.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp1251.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp1252.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp866.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp932.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_cp936.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_cn.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_jp.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_jp_win.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_kr.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_euc_tw.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_htmlent.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_hz.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso2022_kr.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_1.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_10.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_13.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_14.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_15.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_2.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_3.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_4.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_5.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_6.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_7.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_8.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_iso8859_9.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_jis.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_koi8r.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_pass.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_armscii8.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_qprint.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_sjis.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ucs2.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_ucs4.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_uhc.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf16.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf32.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf7.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf7imap.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_utf8.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\mbfilter_uuencode.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfilter_wchar.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_allocators.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_consts.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_convert.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_defs.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_encoding.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_filter_output.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_ident.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_language.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_memory_device.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\mbfl\mbfl_string.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_de.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_en.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_ja.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_kr.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_neutral.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_ru.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_uni.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_zh.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_hy.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\nls\nls_tr.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_prop.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_big5.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cns11643.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cp1251.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cp1252.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cp866.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cp932_ext.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_cp936.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_10.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_13.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_14.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_15.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_16.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_2.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_3.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_4.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_5.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_6.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_7.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_8.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_iso8859_9.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_jis.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_koi8r.h
# End Source File
# Begin Source File

SOURCE=.\libmbfl\filters\unicode_table_uhc.h
# End Source File
# End Group
# End Group
# End Target
# End Project
