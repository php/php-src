--TEST--
Phar: default web stub, no phar extension
--SKIPIF--
<?php if (extension_loaded("phar")) die("skip");?>
--INI--
error_reporting=E_ALL & ^E_NOTICE
--ENV--
SCRIPT_NAME=/nophar_web.php/
REQUEST_URI=/nophar_web.php/
--FILE_EXTERNAL--
nophar.phar
--EXPECT--
web
