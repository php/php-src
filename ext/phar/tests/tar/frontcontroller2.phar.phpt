--TEST--
Phar front controller PHP test tar-based
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller2.phar.php
REQUEST_URI=/frontcontroller2.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
