PHP 4.0 - Win32 Distribution
============================


Note
----

PHP 4.0 for Windows comes in two flavours - a CGI executable (php.exe), and an
ISAPI module (php4isapi.dll).  The latter is new to PHP 4.0, and provides
significantly improved performance and some new functionality.  However,
please note that the ISAPI module is *NOT* yet considered to be production
quality.  The reason for this is that the PHP ISAPI module uses the
thread-safe version of the PHP code, which is completely new to PHP 4.0, and
has not yet been tested and pounded enough to be considered completely stable,
and there are actually a few known bugs.  On the other hand, some people have
reported very good results with the ISAPI module, even though we're not aware
of anyone actually running it on a production site.  In short - your mileage
may vary;  If you need absolute stability, trade the performance of the ISAPI
module with the stability of the CGI executable.


Requirements
------------

- Windows 95 or later, Windows NT 4.0 or later
- ISAPI 4.0 compliant Web server required for ISAPI module (tested on IIS 4.0,
  PWS 4.0 IIS 5.0).  IIS 3.0 is *NOT* supported;  You should download and
  install the Windows NT 4.0 Option Pack with IIS 4.0 if you want native PHP
  support.
- Windows 95 requires the DCOM update, freely available from:
    http://download.microsoft.com/msdownload/dcom/95/x86/en/dcom95.exe


Installation
------------

Unlike PHP 3, PHP 4 is divided into several components;  Even when you use
PHP in its CGI mode, php.exe is no longer a standalone executable, and relies
on external DLLs for execution.  For this reason, if you install php.exe as
a handler for your web server, you must make sure that the DLLs in the
distribution exist in a directory that is in the Windows PATH.  The easiest
way to do it is to copy these DLLs to your SYSTEM (Windows 9x) or SYSTEM32
(Windows NT) directory, which is under your Windows directory.
The DLLs that need to be copied are MSVCRT.DLL (it may already exist in there)
and PHP4TS.DLL.

Installation Notes for IIS 4.0 and 5.0
--------------------------------------

- Install the DLLs as mentioned above.
- If you wish to change any defaults, copy either the php.ini-dist or
  php.ini-optimized files into your Windows directory, and rename it to
  'php.ini'.  You can then modify any of the directives inside it.
- Start the Microsoft Management Console (may appear as
  'Internet Services Manager', either in your Windows NT 4.0 Option Pack
  branch or the Control Panel->Administrative Tools under Windows 2000).
- Right click on your Web server node (will most probably appear as 'Default
  Web Server'), and select 'Properties'.
- If you don't want to perform HTTP Authentication using PHP, you can (and
  should) skip this step.  Under 'ISAPI Filters', add a new ISAPI filter.
  Use 'PHP' as the filter name, and supply a path to the php4isapi.dll that
  is included in the distribution.
- Under 'Home Directory', click on the 'Configuration' button.  Add a new
  entry to the Application Mappings;  Use the path the php4isapi.dll as the
  Executable, supply .php as the extension, leave 'Method exclusions', blank,
  and check the Script engine checkbox.
- Stop IIS completely (you would have to stop the IIS Administration service
  for that - stopping it through the Management Console isn't good enough;
  You can do it by typing 'net stop iisadmin' on a command prompt).
- Start IIS again (you can do it by typing 'net start w3svc' on a command
  prompt).
- Put a .php file under your Web server's document root and check if it works!



Installation Notes for Microsoft PWS 4.0
----------------------------------------

- Install the php.ini file and the DLLs as mentioned above.
- Edit the enclosed PWS-php4.reg file to reflect the location of your
  php4isapi.dll.  Forward slashes should be escaped, for example:
  
[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\w3svc\parameters\Script Map]
".php"="C:\\Program Files\\PHP\\php4isapi.dll"

- In the PWS Manager, right click on a given directory you want to add PHP
  support to, and select Properties.  Check the 'Execute' checkbox, and
  confirm.

At this point, PWS should have built-in PHP support.



Adding Extensions
-----------------

A few pre-compiled extensions are included in this release.  You can load them
by adding a line like the following to your php.ini file:
extension=path\to\extension\extension_name.dll
Loading these extensions extends the functionality of PHP to support more
functions and protocols.
Note:  MySQL support is *built-in* as of PHP 4.0.  You don't need to load any
       additional extensions in order to use the MySQL functions.

Good luck,

Zeev <zeev@zend.com>, Andi <andi@zend.com>  [June 25, 2000]
