--TEST--
Phar front controller with generic action router test
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller23.php
REQUEST_URI=/frontcontroller23.php/hi/there
PATH_INFO=/hi/there
--FILE_EXTERNAL--
files/frontcontroller14.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECTF--
string(9) "/hi/there"
string(%d) "phar://%sfrontcontroller23.php/html/index.php"