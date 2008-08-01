--TEST--
Phar front controller with unknown extension mime type
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller26.php
REQUEST_URI=/frontcontroller26.php/unknown.ext
PATH_INFO=/unknown.ext
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: application/octet-stream
--EXPECTF--
<?php var_dump("hi");