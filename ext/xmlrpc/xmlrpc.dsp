# Microsoft Developer Studio Project File - Name="xmlrpc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xmlrpc - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xmlrpc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xmlrpc.mak" CFG="xmlrpc - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xmlrpc - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xmlrpc - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmlrpc - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XMLRPC_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "libxmlrpc" /I "..\..\bundle\expat" /D HAVE_XMLRPC=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D ZEND_DEBUG=1 /D ZTS=1 /D COMPILE_DL_XMLRPC=1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XMLRPC_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php5ts_debug.lib expat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_xmlrpc.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "xmlrpc - Win32 Release_TS"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XMLRPC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /I "libxmlrpc" /I "..\..\bundle\expat" /D HAVE_XMLRPC=1 /D "ZEND_WIN32" /D ZEND_DEBUG=0 /D "PHP_WIN32" /D ZTS=1 /D COMPILE_DL_XMLRPC=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XMLRPC_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php5ts.lib expat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_xmlrpc.dll" /libpath:"..\..\Release_TS"

!ENDIF 

# Begin Target

# Name "xmlrpc - Win32 Debug_TS"
# Name "xmlrpc - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\xmlrpc-epi-php.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\php_xmlrpc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libxmlrpc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libxmlrpc\base64.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\base64.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\encodings.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\encodings.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\queue.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\queue.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\simplestring.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\simplestring.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\system_methods.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\system_methods_private.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_element.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_element.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_dandarpc.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_dandarpc.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_soap.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_soap.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_xmlrpc.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xml_to_xmlrpc.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc_introspection.c
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc_introspection.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc_introspection_private.h
# End Source File
# Begin Source File

SOURCE=.\libxmlrpc\xmlrpc_private.h
# End Source File
# End Group
# End Target
# End Project
