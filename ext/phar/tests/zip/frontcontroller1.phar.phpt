--TEST--
Phar front controller other zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller1.phar.php/a.jpg
REQUEST_URI=/frontcontroller1.phar.php/a.jpg
--FILE_EXTERNAL--
frontcontroller.phar.zip
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio
