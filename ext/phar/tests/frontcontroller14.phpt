--TEST--
Phar front controller mime type override, other
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller14.php/a.jpg
REQUEST_URI=/frontcontroller14.php/a.jpg
--FILE_EXTERNAL--
frontcontroller8.phar
--EXPECTHEADERS--
Content-type: foo/bar
Content-length: 4
--EXPECT--
hio2
