nsapi configuration file information

netscape config files are located in:
/netscape/suitespot/httpd-servername/config

add the following line to mime.types

type=magnus-internal/x-httpd-php	exts=php


Add the following to obj.conf

#note place following two lines after mime types init!
Init fn="load-modules" funcs="php4_init,php4_close,php4_execute,php4_auth_trans" shlib="/php4/nsapiPHP4.dll"
Init fn=php4_init errorString="Failed to initialize PHP!"

<Object name="default">
.
.
.
.#NOTE this next line should happen after all 'ObjectType' and before all 'AddLog' lines
Service fn="php4_execute" type="magnus-internal/x-httpd-php"
.
.
</Object>


<Object name="x-httpd-php">
ObjectType fn="force-type" type="magnus-internal/x-httpd-php"
Service fn=php4_execute
</Object>


Authentication configuration

PHP authentication cannot be used with any other authentication.  ALL AUTHENTICATION IS
PASSED TO YOUR PHP SCRIPT.  To configure PHP Authentication for the entire server, add
the following line:

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
