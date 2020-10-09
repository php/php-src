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
if (getenv('SKIP_ASAN')) die('xfail Startup failure leak');
?>
--FILE--
<?php
var_dump(trait_exists('T'));
var_dump(class_exists('Foo'));
?>
--EXPECT--
Fatal error: Undefined constant "UNDEF" in Unknown on line 0

Fatal error: Failed to resolve initializers of class Foo during preloading in Unknown on line 0
