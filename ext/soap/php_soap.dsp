# Microsoft Developer Studio Project File - Name="php_soap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php_soap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php_soap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php_soap.mak" CFG="php_soap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php_soap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php_soap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php_soap - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\php4" /I "..\..\..\php4\Zend" /I "..\..\..\php4\TSRM" /I "..\..\..\php4\main" /I "..\..\..\libxml2-2.4.12\include" /I "..\..\bind" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTSWS" /D "PHP_SOAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "ZTS" /D ZEND_DEBUG=0 /D "COMPILE_DL_SOAP" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib libxml2.lib wsock32.lib resolv.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS\php_soap.dll" /libpath:"..\..\..\php4\Release_TS" /libpath:"..\..\..\libxml2-2.4.12\lib"

!ELSEIF  "$(CFG)" == "php_soap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\php4" /I "..\..\..\php4\Zend" /I "..\..\..\php4\TSRM" /I "..\..\..\php4\main" /I "..\..\..\libxml2-2.4.12\include" /I "..\..\..\bind" /D "WS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "ZTS" /D ZEND_DEBUG=1 /D "COMPILE_DL_SOAP" /FR"Release_TS/" /Fp"Release_TS/gd.pch" /YX /Fo"Release_TS/" /Fd"Release_TS/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts_debug.lib libxml2.lib wsock32.lib resolv.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS\php_soap.dll" /implib:"Release_TS/php_gd.lib" /pdbtype:sept /libpath:"..\..\..\php4\Debug_TS" /libpath:"..\..\..\libxml2-2.4.12\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "php_soap - Win32 Release"
# Name "php_soap - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\php_encoding.c
# End Source File
# Begin Source File

SOURCE=.\php_http.c
# End Source File
# Begin Source File

SOURCE=.\php_packet_soap.c
# End Source File
# Begin Source File

SOURCE=.\php_schema.c
# End Source File
# Begin Source File

SOURCE=.\php_sdl.c
# End Source File
# Begin Source File

SOURCE=.\php_xml.c
# End Source File
# Begin Source File

SOURCE=.\soap.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_encoding.h
# End Source File
# Begin Source File

SOURCE=.\php_http.h
# End Source File
# Begin Source File

SOURCE=.\php_packet_soap.h
# End Source File
# Begin Source File

SOURCE=.\php_schema.h
# End Source File
# Begin Source File

SOURCE=.\php_sdl.h
# End Source File
# Begin Source File

SOURCE=.\php_soap.h
# End Source File
# Begin Source File

SOURCE=.\php_xml.h
# End Source File
# End Group
# End Target
# End Project
