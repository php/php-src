--TEST--
Preloading classes linked with traits
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
Y::foo();
Y::bar();
?>
OK
--EXPECT--
string(7) "T1::foo"
string(7) "T2::bar"
OK
