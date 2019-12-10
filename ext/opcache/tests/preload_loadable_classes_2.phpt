--TEST--
Preloading: Loadable class checking (2)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_2.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
Unreachable
--EXPECTF--
Warning: Use of undefined constant UNDEF - assumed 'UNDEF' (this will throw an Error in a future version of PHP) in Unknown on line 0

Fatal error: Class 'Foo' not found in Unknown on line 0

Fatal error: Failed to resolve initializers of class Test during preloading in Unknown on line 0
