--TEST--
Phar front controller with valid callback that does not return any value
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller33.php
REQUEST_URI=/frontcontroller33.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller18.phar
--EXPECT--
phar error: rewrite callback must return a string or false
