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
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MBSTRING_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D ZEND_DEBUG=0 /D "_MBCS" /D "_USRDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_MBSTRING" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D MBSTRING_EXPORTS=1 /FR /YX /FD /c
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "MBSTRING_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "COMPILE_DL_MBSTRING" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D MBSTRING_EXPORTS=1 /YX /FD /GZ /c
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

SOURCE=.\html_entities.c
# End Source File
# Begin Source File

SOURCE=.\mb_gpc.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter_cn.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter_ja.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter_kr.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter_ru.c
# End Source File
# Begin Source File

SOURCE=.\mbfilter_tw.c
# End Source File
# Begin Source File

SOURCE=.\mbregex.c
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

SOURCE=.\cp932_table.h
# End Source File
# Begin Source File

SOURCE=.\mb_gpc.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter_cn.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter_ja.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter_kr.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter_ru.h
# End Source File
# Begin Source File

SOURCE=.\mbfilter_tw.h
# End Source File
# Begin Source File

SOURCE=.\mbregex.h
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
# Begin Source File

SOURCE=.\unicode_table.h
# End Source File
# Begin Source File

SOURCE=.\unicode_table_cn.h
# End Source File
# Begin Source File

SOURCE=.\unicode_table_kr.h
# End Source File
# Begin Source File

SOURCE=.\unicode_table_tw.h
# End Source File
# End Group
# End Target
# End Project
