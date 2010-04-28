# Microsoft Developer Studio Project File - Name="dom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dom - Win32 Release_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dom.mak" CFG="dom - Win32 Release_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dom - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dom - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dom - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\..\Zend" /I "..\..\..\TSRM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_DOM" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /D ZEND_DEBUG=0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOM_EXPORTS" /D "COMPILE_DL_DOM" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_DOM=1 /D "LIBXML_THREAD_ENABLED" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 wsock32.lib php5ts.lib resolv.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /machine:I386 /out:"..\..\Release_TS/php_dom.dll" /implib:"..\..\Release_TS/php_dom.lib" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline" /libpath:"..\..\..\bindlib_w32\Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dom - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "mssql-70" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_DOM" /D ZTS=1 /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOM_EXPORTS" /D "COMPILE_DL_DOM" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_DOM=1 /D LIBXML_THREAD_ENABLED=1 /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts_debug.lib ws2_32.lib resolv.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"..\..\Debug_TS\php_dom.dll" /implib:"..\..\Debug_TS/php_dom.lib" /libpath:"..\..\Debug_TS" /libpath:"..\..\..\bindlib_w32\Release"

!ENDIF 

# Begin Target

# Name "dom - Win32 Release_TS"
# Name "dom - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\attr.c
# End Source File
# Begin Source File

SOURCE=.\cdatasection.c
# End Source File
# Begin Source File

SOURCE=.\characterdata.c
# End Source File
# Begin Source File

SOURCE=.\comment.c
# End Source File
# Begin Source File

SOURCE=.\document.c
# End Source File
# Begin Source File

SOURCE=.\documentfragment.c
# End Source File
# Begin Source File

SOURCE=.\documenttype.c
# End Source File
# Begin Source File

SOURCE=.\domconfiguration.c
# End Source File
# Begin Source File

SOURCE=.\domerror.c
# End Source File
# Begin Source File

SOURCE=.\domerrorhandler.c
# End Source File
# Begin Source File

SOURCE=.\domexception.c
# End Source File
# Begin Source File

SOURCE=.\domimplementation.c
# End Source File
# Begin Source File

SOURCE=.\domimplementationlist.c
# End Source File
# Begin Source File

SOURCE=.\domimplementationsource.c
# End Source File
# Begin Source File

SOURCE=.\domlocator.c
# End Source File
# Begin Source File

SOURCE=.\domstringlist.c
# End Source File
# Begin Source File

SOURCE=.\element.c
# End Source File
# Begin Source File

SOURCE=.\entity.c
# End Source File
# Begin Source File

SOURCE=.\entityreference.c
# End Source File
# Begin Source File

SOURCE=.\namednodemap.c
# End Source File
# Begin Source File

SOURCE=.\namelist.c
# End Source File
# Begin Source File

SOURCE=.\node.c
# End Source File
# Begin Source File

SOURCE=.\nodelist.c
# End Source File
# Begin Source File

SOURCE=.\notation.c
# End Source File
# Begin Source File

SOURCE=.\php_dom.c
# End Source File
# Begin Source File

SOURCE=.\processinginstruction.c
# End Source File
# Begin Source File

SOURCE=.\string_extend.c
# End Source File
# Begin Source File

SOURCE=.\text.c
# End Source File
# Begin Source File

SOURCE=.\typeinfo.c
# End Source File
# Begin Source File

SOURCE=.\userdatahandler.c
# End Source File
# Begin Source File

SOURCE=.\xpath.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dom_ce.h
# End Source File
# Begin Source File

SOURCE=.\dom_fe.h
# End Source File
# Begin Source File

SOURCE=.\dom_properties.h
# End Source File
# Begin Source File

SOURCE=.\php_dom.h
# End Source File
# Begin Source File

SOURCE=.\xml_common.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
