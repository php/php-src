--TEST--
Phar front controller with extra path_info
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller25.php
REQUEST_URI=/frontcontroller25.php/a1.phps/extra/stuff
PATH_INFO=/a1.phps/extra/stuff
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
string(42) "/frontcontroller25.php/a1.phps/extra/stuff"
string(12) "/extra/stuff"
