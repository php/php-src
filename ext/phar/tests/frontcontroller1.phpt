--TEST--
Phar front controller other
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller1.php/a.jpg
REQUEST_URI=/frontcontroller1.php/a.jpg
--FILE_EXTERNAL--
frontcontroller.phar
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio
