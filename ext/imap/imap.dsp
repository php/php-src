# Microsoft Developer Studio Project File - Name="imap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=imap - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "imap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imap.mak" CFG="imap - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "imap - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "IMAP_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 c-client.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib wsock32.lib winmm.lib /nologo /dll /machine:I386 /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "IMAP_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 c-client.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib wsock32.lib winmm.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug_TS"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "ZTS" /D "IMAP_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib wsock32.lib winmm.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\\" /libpath:"..\..\Debug_TS"

!ELSEIF  "$(CFG)" == "imap - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "ZTS" /D "IMAP_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILE_DL_IMAP" /D HAVE_IMAP=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 c-client.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib wsock32.lib winmm.lib /nologo /dll /machine:I386 /libpath:"..\..\Debug_TS"

!ENDIF 

# Begin Target

# Name "imap - Win32 Release"
# Name "imap - Win32 Debug"
# Name "imap - Win32 Debug_TS"
# Name "imap - Win32 Release_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\imap.c

!IF  "$(CFG)" == "imap - Win32 Release"

# ADD CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug"

# ADD CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"

!ELSEIF  "$(CFG)" == "imap - Win32 Debug_TS"

# ADD BASE CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"
# ADD CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"

!ELSEIF  "$(CFG)" == "imap - Win32 Release_TS"

# ADD BASE CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"
# ADD CPP /I "..\.." /I "..\..\Zend" /I "..\..\..\bindlib_w32" /I "..\..\..\IMAP\c-client" /I "..\..\TSRM"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\imap.h
# End Source File
# End Group
# Begin Group "IMAP"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\Imap\c-client\dummynt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\fdstring.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\flstring.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\imap4r1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\mail.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\mbxnt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\misc.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\mtxnt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\netmsg.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\newsrc.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\nntp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\os_nt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\pop3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\pseudo.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\rfc822.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\smanager.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\smtp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\tenexnt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\unixnt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\Imap\c-client\utf8.c"
# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter ""
# End Group
# End Group
# End Target
# End Project
