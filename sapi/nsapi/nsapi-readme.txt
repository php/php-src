Configuration of your Netscape/SunONE/iPlanet Web Server for PHP4
-------------------------------------------------------------

These instructions are targetted at Netscape Enterprise Web Server and
SUN/Netscape Alliance iPlanet Web Server/SunONE Webserver.
On other web servers your milage may vary.

Firstly you may need to add some paths to the LD_LIBRARY_PATH
environment for Netscape to find all the shared libs. This is best done
in the start script for your Netscape server.  Windows users can
probably skip this step. The start script is located in:

    <path-to-netscape-server>/https-servername/start


Netscape/iPlanet/SunONE config files are located in:

    <path-to-server>/https-servername/config


Add the following line to mime.types (you can do that by the administration server):

    type=magnus-internal/x-httpd-php	exts=php


Place the following two lines after mime.types init in
<path-to-server>/https-servername/config/obj.conf (for servers < 6) or
for iPlanet/SunONE Web Server 6.0 and above however at the end of the
<path-to-server>/https-servername/config/magnus.conf file:

    Init fn="load-modules" funcs="php4_init,php4_execute,php4_auth_trans" shlib="/path/to/phplibrary"
    Init fn=php4_init errorString="Failed to initialize PHP!" [php_ini="/path/to/php.ini"]

The "shlib" will vary depending on your OS:

	Unix: "<path-to-server>/bin/libphp4.so".
	Windows: "c:/path/to/PHP4/php4nsapi.dll"


In obj.conf (for virtual server classes [SunONE 6.0] in their vserver.obj.conf):

    <Object name="default">
    .
    .
    .
    # NOTE this next line should happen after all 'ObjectType' and before
    # all 'AddLog' lines
    # You can modify some entries in php.ini request specific by adding it to the Service
    # directive, e.g. doc_root="/path"
    # For boolean ini-keys please use 0/1 as value, NOT "On","Off",... (this will not work
    # correctly), e.g. zlib.output_compression=1 instead of zlib.output_compression="On"

    Service fn="php4_execute" type="magnus-internal/x-httpd-php" [inikey=value ...]
    .
    .
    .
    </Object>

This is only needed if you want to configure a directory that only consists of
PHP scripts (same like a cgi-bin directory):

    <Object name="x-httpd-php">
    ObjectType fn="force-type" type="magnus-internal/x-httpd-php"
    Service fn="php4_execute" [inikey=value ...]
    </Object>

After that you can configure a directory in the Administration server and assign it
the style "x-httpd-php". All files in it will get executed as PHP. This is nice to
hide PHP usage by renaming files to .html


Authentication configuration
----------------------------

PHP authentication cannot be used with any other authentication. ALL
AUTHENTICATION IS PASSED TO YOUR PHP SCRIPT.  To configure PHP
Authentication for the entire server, add the following line:

    <Object name="default">
    AuthTrans fn=php4_auth_trans
    .
    .
    .
    .
    </Object>


To use PHP Authentication on a single directory, add the following:

    <Object ppath="d:\path\to\authenticated\dir\*">
    AuthTrans fn=php4_auth_trans
    </Object>


Special use for error pages or self-made directory listings
-----------------------------------------------------------

You can use PHP to generate the error pages for "404 Not Found"
or similar. Add the following line to the object in obj.conf for
every error page you want to overwrite:

    Error fn="php4_execute" code=XXX script="/path/to/script.php" [inikey=value inikey=value...]

where XXX ist the HTTP error code. Please delete any other Error
directives which could interfere with yours.
If you want to place a page for all errors that could exist, leave
the "code" parameter out. Your script can get the HTTP status code
with $_SERVER['ERROR_TYPE'].

Another posibility is to generate self-made directory listings.
Just generate a PHP script which displays a directory listing and
replace the corresponding default Service line for
type="magnus-internal/directory" in obj.conf with the following:

    Service fn="php4_execute" type="magnus-internal/directory" script="/path/to/script.php" [inikey=value inikey=value...]

For both error and directory listing pages the original URI and
translated URI are in the variables $_SERVER['PATH_INFO'] and
$_SERVER['PATH_TRANSLATED'].

$Id$
