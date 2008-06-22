--TEST--
Phar front controller with valid callback that is not good [cache_list]
--INI--
default_charset=
phar.cache_list={PWD}/frontcontroller32.php
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