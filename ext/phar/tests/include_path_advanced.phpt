--TEST--
Phar: include_path advanced code coverage test
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--FILE_EXTERNAL--
files/include_path.phar
--EXPECT--
file1.php
test/file1.php
ok
