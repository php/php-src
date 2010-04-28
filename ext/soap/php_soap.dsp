# Microsoft Developer Studio Project File - Name="php_soap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php_soap - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php_soap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php_soap.mak" CFG="php_soap - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php_soap - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php_soap - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php_soap - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /I "..\..\..\libxml2-2.4.12\include" /I "..\..\bind" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTSWS" /D "PHP_SOAP_EXPORTS" /D "ZEND_WIN32" /D "PHP_WIN32" /D "ZTS" /D ZEND_DEBUG=0 /D "COMPILE_DL_SOAP" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib libxml2.lib wsock32.lib resolv.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS\php_soap.dll" /libpath:"..\..\\" /libpath:"..\..\..\libxml2-2.4.12\lib" /libpath:"..\..\Release_TS"

!ELSEIF  "$(CFG)" == "php_soap - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /I "..\..\..\libxml2-2.4.12\include" /I "..\..\bind" /D "WS" /D "_MBCS" /D "_USRDLL" /D "PHP_SOAP_EXPORTS" /D "WIN32" /D "ZEND_WIN32" /D "PHP_WIN32" /D "ZTS" /D ZEND_DEBUG=1 /D "COMPILE_DL_SOAP" /FR"Debug_TS/" /Fp"Debug_TS/soap.pch" /YX /Fo"Debug_TS/" /Fd"Debug_TS/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts_debug.lib libxml2.lib wsock32.lib resolv.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS\php_soap.dll" /pdbtype:sept /libpath:"..\..\\" /libpath:"..\..\..\libxml2-2.4.12\lib" /libpath:"..\..\Debug_TS"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "php_soap - Win32 Release_TS"
# Name "php_soap - Win32 Debug_TS"
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
