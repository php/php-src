--TEST--
GH-9968: Preload crash on non-linked class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/gh9968-1.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

new Root1_Constant_Referencer();

?>
==DONE==
--EXPECTF--
Warning: Can't preload unlinked class Root1_Constant_Referencer: Unknown parent Root2_Empty_Empty in %s on line %d

Fatal error: Uncaught Error: Class "Root1_Constant_Referencer" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
