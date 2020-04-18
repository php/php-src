--TEST--
Bug #78918.2: Class alias during preloading causes assertion failure
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_class_alias_2.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(class_exists('B'));
var_dump(class_exists('C'));
?>
--EXPECT--
bool(true)
bool(true)
