--TEST--
Phar front controller with valid callback that is not good
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller32.php
REQUEST_URI=/frontcontroller32.php
--EXPECTHEADERS--
Content-type: text/html
--FILE_EXTERNAL--
files/frontcontroller17.phar
--EXPECTF--
Warning: Parameter 1 to sort() expected to be a reference, value given in %sfrontcontroller32.php on line 3
phar error: failed to call rewrite callback