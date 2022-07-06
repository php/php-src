--TEST--
Preload: Unresolved property type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_unresolved_prop_type.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
===DONE===
--EXPECTF--
Warning: Can't preload class Test with unresolved property types in %s on line %d
===DONE===
