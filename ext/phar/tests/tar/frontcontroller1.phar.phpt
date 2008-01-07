--TEST--
Phar front controller other tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller1.phar.php/a.jpg
REQUEST_URI=/frontcontroller1.phar.php/a.jpg
--FILE_EXTERNAL--
frontcontroller.phar.tar
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio
