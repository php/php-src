Configuration of your Netscape or iPlanet Web Server for PHP4
-------------------------------------------------------------

These instructions are targetted at Netscape Enterprise Web Server and
SUN/Netscape Alliance iPlanet Web Server. On other web servers your
milage may vary.

Firstly you may need to add some paths to the LD_LIBRARY_PATH
environment for Netscape to find all the shared libs. This is best done
in the start script for your Netscape server.  Windows users can
probably skip this step. The start script is located in:

    <path-to-netscape-server>/https-servername/start


Netscape config files are located in:

    <path-to-netscape-server>/https-servername/config


Add the following line to mime.types:

    type=magnus-internal/x-httpd-php	exts=php


Add the following to obj.conf (for iPlanet/SunONE Web Server 6.0 and above however, you need to make the specified changes to the Init function
in the server-id/config/magnus.conf file, and not the server-id/config/obj.conf file):

"shlib" will vary depending on your OS:

	Unix: "<path-to-netscape-server>/bin/libphp4.so".
	Windows: "c:\path\to\PHP4\nsapiPHP4.dll"


Note! Place following two lines after mime.types init:

    Init fn="load-modules" funcs="php4_init,php4_close,php4_execute,php4_auth_trans" shlib="/php4/nsapiPHP4.dll"
    Init fn=php4_init errorString="Failed to initialize PHP!"

    <Object name="default">
    .
    .
    .
    # NOTE this next line should happen after all 'ObjectType' and before
    # all 'AddLog' lines

    Service fn="php4_execute" type="magnus-internal/x-httpd-php"
    .
    .
    </Object>

    <Object name="x-httpd-php">
    ObjectType fn="force-type" type="magnus-internal/x-httpd-php"
    Service fn=php4_execute
    </Object> 


Authentication configuration
----------------------------

PHP authentication cannot be used with any other authentication.  ALL
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

