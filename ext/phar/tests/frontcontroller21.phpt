--TEST--
Phar front controller $_SERVER munging success
--INI--
default_charset=UTF-8
cgi.fix_pathinfo=1
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller21.php
REQUEST_URI=/frontcontroller21.php/index.php?test=hi
PATH_INFO=/index.php
QUERY_STRING=test=hi
--FILE_EXTERNAL--
files/frontcontroller12.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
string(10) "/index.php"
string(10) "/index.php"
string(%d) "phar://%sfrontcontroller21.php/index.php"
string(18) "/index.php?test=hi"
string(32) "/frontcontroller21.php/index.php"
string(22) "/frontcontroller21.php"
string(%d) "%sfrontcontroller21.php"
string(40) "/frontcontroller21.php/index.php?test=hi"
