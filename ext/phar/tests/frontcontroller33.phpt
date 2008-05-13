--TEST--
Phar front controller with valid callback that does not return any value
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller33.php
REQUEST_URI=/frontcontroller33.php
--EXPECTHEADERS--
Content-type: text/html
--FILE_EXTERNAL--
files/frontcontroller18.phar
--EXPECTF--
phar error: rewrite callback must return a string or false