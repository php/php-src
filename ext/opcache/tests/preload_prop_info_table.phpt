--TEST--
Preloading of the property info table with internal parent
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$e = new MyException("foo");
echo $e->getMessage(), "\n";
?>
--EXPECT--
foo
