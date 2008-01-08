--TEST--
Phar front controller PHP test zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller2.phar.php/a.php
REQUEST_URI=/frontcontroller2.phar.php/a.php
--FILE_EXTERNAL--
frontcontroller.phar.zip
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
