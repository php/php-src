--TEST--
Phar: include_path advanced code coverage test
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/include_path.phar
--EXPECT--
file1.php
test/file1.php
ok