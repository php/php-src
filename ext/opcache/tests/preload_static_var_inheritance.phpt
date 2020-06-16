--TEST--
Bug #79548: Preloading segfault with inherited method using static variable
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_static_var_inheritance.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump((new B)->test());
?>
--EXPECT--
NULL
