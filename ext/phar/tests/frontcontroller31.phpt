--TEST--
Phar front controller with invalid callback for rewrites
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller31.php
REQUEST_URI=/frontcontroller31.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller16.phar
--EXPECT--
phar error: invalid rewrite callback