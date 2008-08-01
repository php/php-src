--TEST--
Phar front controller with generic action router test [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller23.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller23.php
REQUEST_URI=/frontcontroller23.php/hi/there
PATH_INFO=/hi/there
--FILE_EXTERNAL--
files/frontcontroller14.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
string(9) "/hi/there"
string(%d) "phar://%sfrontcontroller23.php/html/index.php"