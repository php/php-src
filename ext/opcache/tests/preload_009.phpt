--TEST--
Preloading class using trait with undefined class constant access
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_undef_const_2.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(trait_exists('T'));
var_dump(class_exists('Foo'));
?>
--EXPECTF--
Warning: Use of undefined constant UNDEF - assumed 'UNDEF' (this will throw an Error in a future version of PHP) in Unknown on line 0
bool(true)
bool(true)
