--TEST--
Phar: default web stub, no phar extension
--SKIPIF--
<?php if (extension_loaded("phar")) die("skip");?>
--ENV--
SCRIPT_NAME=/nophar.phar
REQUEST_URI=/nophar.phar
--FILE_EXTERNAL--
nophar.phar
--EXPECT--
web
