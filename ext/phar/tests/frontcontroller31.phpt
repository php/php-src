--TEST--
Phar front controller with invalid callback for rewrites
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller31.php
REQUEST_URI=/frontcontroller31.php
--EXPECTHEADERS--
Content-type: text/html
--FILE_EXTERNAL--
files/frontcontroller16.phar
--EXPECT--
phar error: invalid rewrite callback