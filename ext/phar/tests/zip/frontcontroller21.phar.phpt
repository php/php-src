--TEST--
Phar front controller $_SERVER munging success zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller21.phar.php/index.php
REQUEST_URI=/frontcontroller21.phar.php/index.php?test=hi
--FILE_EXTERNAL--
frontcontroller12.phar.zip
--EXPECTHEADERS--
Content-type: text/html
--EXPECTF--
string(10) "/index.php"
string(%d) "phar://%sfrontcontroller21.phar.php/index.php"
string(%d) "phar://%sfrontcontroller21.phar.php/index.php"
string(18) "/index.php?test=hi"
string(37) "/frontcontroller21.phar.php/index.php"
string(37) "/frontcontroller21.phar.php/index.php"
string(%d) "%sfrontcontroller21.phar.php"
string(45) "/frontcontroller21.phar.php/index.php?test=hi"