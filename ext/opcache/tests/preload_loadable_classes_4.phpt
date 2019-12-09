--TEST--
Preloading: Loadable class checking (4)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_4.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY != 'Windows') die('skip Windows only');
?>
--FILE--
Unreachable
--EXPECTF--
Fatal error: Class Test uses internal class Exception during preloading, which is not supported on Windows in Unknown on line 0
