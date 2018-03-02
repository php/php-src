--TEST--
Phar front controller with weird SCRIPT_NAME
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip phar extension not loaded"); ?>
--ENV--
SCRIPT_NAME=/huh?
REQUEST_URI=/huh?
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTF--
oops did not run
%a