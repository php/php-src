--TEST--
Phar front controller PHP test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller2.php
REQUEST_URI=/frontcontroller2.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
