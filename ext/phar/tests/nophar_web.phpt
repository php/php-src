--TEST--
Phar: default web stub, no phar extension
--SKIPIF--
<?php if (extension_loaded("phar")) die("skip Phar extension must be disabled for this test"); ?>
--ENV--
SCRIPT_NAME=/nophar.phar
REQUEST_URI=/nophar.phar
--FILE_EXTERNAL--
files/nophar.phar
--EXPECT--
web
