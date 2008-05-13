--TEST--
Phar front controller with valid callback that is not good
--INI--
default_charset=
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
%ahar error: failed to call rewrite callback