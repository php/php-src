# Microsoft Developer Studio Project File - Name="php4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=php4 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php4.mak" CFG="php4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php4 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "php4 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "cgi_release"
# PROP Intermediate_Dir "cgi_release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "c:\projects\php4\include" /I "c:\projects\zend\zendcore" /I "." /I "regex\\" /I "d:\src\bind\include" /I "c:\projects\sapi" /D "NDEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "COMPILE_LIBZEND" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /machine:I386 /out:"cgi_release\php.exe" /libpath:"c:\Projects\php4\lib" /libpath:"\src\lib" /libpath:"d:\src\bind\lib" /libpath:"c:\projects\sapi\release" /libpath:"c:\projects\zend\zendcore\release"

!ELSEIF  "$(CFG)" == "php4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\projects\zend\cgi_debug"
# PROP Intermediate_Dir "cgi_debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\projects\php4\include" /I "c:\projects\libzend" /I "." /I "regex\\" /I "d:\src\bind\include" /I "c:\projects\sapi" /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "COMPILE_LIBZEND" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SAPI.lib libzend.lib /nologo /version:3.0 /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcd" /out:"cgi_debug\php.exe" /pdbtype:sept /libpath:"c:\Projects\php4\lib" /libpath:"\src\lib" /libpath:"d:\src\bind\lib" /libpath:"c:\projects\sapi\debug" /libpath:"c:\projects\libzend\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "php4 - Win32 Release"
# Name "php4 - Win32 Debug"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\configuration-parser.c"
# End Source File
# Begin Source File

SOURCE=".\configuration-scanner.c"
# End Source File
# Begin Source File

SOURCE=.\functions\cyr_convert.c
# End Source File
# Begin Source File

SOURCE=".\fopen-wrappers.c"
# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\internal_functions.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\output.c
# End Source File
# Begin Source File

SOURCE=.\php3_realpath.c
# End Source File
# Begin Source File

SOURCE=.\php_ini.c
# End Source File
# Begin Source File

SOURCE=.\functions\quot_print.c
# End Source File
# Begin Source File

SOURCE=.\request_info.c
# End Source File
# Begin Source File

SOURCE=.\safe_mode.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\config.w32.h
# End Source File
# Begin Source File

SOURCE=".\configuration-parser.h"
# End Source File
# Begin Source File

SOURCE=".\configuration-parser.tab.h"
# End Source File
# Begin Source File

SOURCE=.\control_structures.h
# End Source File
# Begin Source File

SOURCE=.\functions\cyr_convert.h
# End Source File
# Begin Source File

SOURCE=.\getopt.h
# End Source File
# Begin Source File

SOURCE=.\internal_functions_registry.h
# End Source File
# Begin Source File

SOURCE=.\logos.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\output.h
# End Source File
# Begin Source File

SOURCE=.\php.h
# End Source File
# Begin Source File

SOURCE=.\php3_compat.h
# End Source File
# Begin Source File

SOURCE=.\php3_realpath.h
# End Source File
# Begin Source File

SOURCE=.\php_globals.h
# End Source File
# Begin Source File

SOURCE=.\php_ini.h
# End Source File
# Begin Source File

SOURCE=.\functions\quot_print.h
# End Source File
# Begin Source File

SOURCE=.\request_info.h
# End Source File
# Begin Source File

SOURCE=.\safe_mode.h
# End Source File
# Begin Source File

SOURCE=.\win95nt.h
# End Source File
# End Group
# End Group
# Begin Group "Function Modules"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\functions\apache.c
# End Source File
# Begin Source File

SOURCE=.\functions\base64.c
# End Source File
# Begin Source File

SOURCE=.\functions\basic_functions.c
# End Source File
# Begin Source File

SOURCE=.\functions\bcmath.c
# End Source File
# Begin Source File

SOURCE=.\functions\browscap.c
# End Source File
# Begin Source File

SOURCE=.\functions\datetime.c
# End Source File
# Begin Source File

SOURCE=.\functions\dir.c
# End Source File
# Begin Source File

SOURCE=.\functions\dl.c
# End Source File
# Begin Source File

SOURCE=.\functions\dns.c
# End Source File
# Begin Source File

SOURCE=.\functions\exec.c
# End Source File
# Begin Source File

SOURCE=.\functions\file.c
# End Source File
# Begin Source File

SOURCE=.\functions\filestat.c
# End Source File
# Begin Source File

SOURCE=.\functions\formatted_print.c
# End Source File
# Begin Source File

SOURCE=.\functions\fsock.c
# End Source File
# Begin Source File

SOURCE=.\functions\head.c
# End Source File
# Begin Source File

SOURCE=.\functions\html.c
# End Source File
# Begin Source File

SOURCE=.\functions\image.c
# End Source File
# Begin Source File

SOURCE=.\functions\info.c
# End Source File
# Begin Source File

SOURCE=.\functions\iptc.c
# End Source File
# Begin Source File

SOURCE=.\functions\link.c
# End Source File
# Begin Source File

SOURCE=.\functions\mail.c
# End Source File
# Begin Source File

SOURCE=.\functions\math.c
# End Source File
# Begin Source File

SOURCE=.\functions\md5.c
# End Source File
# Begin Source File

SOURCE=.\functions\microtime.c
# End Source File
# Begin Source File

SOURCE=.\functions\mime.c
# End Source File
# Begin Source File

SOURCE=.\functions\number.c
# End Source File
# Begin Source File

SOURCE=.\functions\pack.c
# End Source File
# Begin Source File

SOURCE=.\functions\pageinfo.c
# End Source File
# Begin Source File

SOURCE=.\functions\post.c
# End Source File
# Begin Source File

SOURCE=.\functions\rand.c
# End Source File
# Begin Source File

SOURCE=.\functions\reg.c
# End Source File
# Begin Source File

SOURCE=.\functions\soundex.c
# End Source File
# Begin Source File

SOURCE=.\functions\string.c
# End Source File
# Begin Source File

SOURCE=.\functions\syslog.c
# End Source File
# Begin Source File

SOURCE=.\functions\type.c
# End Source File
# Begin Source File

SOURCE=.\functions\unified_odbc.c
# End Source File
# Begin Source File

SOURCE=.\functions\uniqid.c
# End Source File
# Begin Source File

SOURCE=.\functions\url.c
# End Source File
# Begin Source File

SOURCE=.\functions\var.c
# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\functions\base64.h
# End Source File
# Begin Source File

SOURCE=.\functions\basic_functions.h
# End Source File
# Begin Source File

SOURCE=.\functions\datetime.h
# End Source File
# Begin Source File

SOURCE=.\functions\dl.h
# End Source File
# Begin Source File

SOURCE=.\functions\dns.h
# End Source File
# Begin Source File

SOURCE=.\functions\exec.h
# End Source File
# Begin Source File

SOURCE=.\functions\file.h
# End Source File
# Begin Source File

SOURCE=.\functions\global.h
# End Source File
# Begin Source File

SOURCE=.\functions\head.h
# End Source File
# Begin Source File

SOURCE=.\functions\html.h
# End Source File
# Begin Source File

SOURCE=.\functions\mail.h
# End Source File
# Begin Source File

SOURCE=.\functions\md5.h
# End Source File
# Begin Source File

SOURCE=.\functions\mime.h
# End Source File
# Begin Source File

SOURCE=.\functions\number.h
# End Source File
# Begin Source File

SOURCE=.\functions\pageinfo.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_bcmath.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_dir.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_filestat.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_ldap.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_string.h
# End Source File
# Begin Source File

SOURCE=.\functions\php3_syslog.h
# End Source File
# Begin Source File

SOURCE=.\functions\phpdir.h
# End Source File
# Begin Source File

SOURCE=.\functions\phpmath.h
# End Source File
# Begin Source File

SOURCE=.\functions\phpstring.h
# End Source File
# Begin Source File

SOURCE=.\functions\post.h
# End Source File
# Begin Source File

SOURCE=.\functions\reg.h
# End Source File
# Begin Source File

SOURCE=.\functions\type.h
# End Source File
# Begin Source File

SOURCE=.\functions\url.h
# End Source File
# End Group
# Begin Group "Regular Expressions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=.\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=.\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=.\regex\regfree.c
# End Source File
# End Group
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Group "Source Files No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\functions\COM.c
# End Source File
# Begin Source File

SOURCE=.\win32\pwd.c
# End Source File
# Begin Source File

SOURCE=.\win32\readdir.c
# End Source File
# Begin Source File

SOURCE=.\win32\sendmail.c
# End Source File
# Begin Source File

SOURCE=.\win32\time.c
# End Source File
# Begin Source File

SOURCE=.\win32\wfile.c
# End Source File
# Begin Source File

SOURCE=.\dl\snmp\winsnmp.c
# End Source File
# Begin Source File

SOURCE=.\win32\winutil.c
# End Source File
# Begin Source File

SOURCE=.\win32\wsyslog.c
# End Source File
# End Group
# Begin Group "Header Files No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\grp.h
# End Source File
# Begin Source File

SOURCE=.\win32\param.h
# End Source File
# Begin Source File

SOURCE=.\win32\pwd.h
# End Source File
# Begin Source File

SOURCE=.\win32\readdir.h
# End Source File
# Begin Source File

SOURCE=.\win32\sendmail.h
# End Source File
# Begin Source File

SOURCE=.\win32\syslog.h
# End Source File
# Begin Source File

SOURCE=.\win32\time.h
# End Source File
# Begin Source File

SOURCE=.\win32\unistd.h
# End Source File
# Begin Source File

SOURCE=.\win32\wfile.h
# End Source File
# End Group
# End Group
# Begin Group "Parsers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\configuration-parser.y"

!IF  "$(CFG)" == "php4 - Win32 Release"

!ELSEIF  "$(CFG)" == "php4 - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=".\configuration-parser.y"

BuildCmds= \
	bison --output=configuration-parser.c -v -d -S "C:\Program Files\Cygnus\share\bison.simple" -p cfg configuration-parser.y

"$(InputDir)\configuration-parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\configuration-parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Scanners"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\configuration-scanner.l"

!IF  "$(CFG)" == "php4 - Win32 Release"

!ELSEIF  "$(CFG)" == "php4 - Win32 Debug"

# Begin Custom Build
InputPath=".\configuration-scanner.l"

"configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -i -Pcfg -oconfiguration-scanner.c configuration-scanner.l

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
