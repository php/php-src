--TEST--
Bug #78376 (Incorrect preloading of constant static properties)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78376.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
const CNST = 'bbbb';
var_dump(\A::$a);
?>
--EXPECT--
string(4) "bbbb"
