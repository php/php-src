--TEST--
Bug #78014 (Preloaded classes may depend on non-preloaded classes due to unresolved consts)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78014.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
class B extends A {
    function foo(): int { return 24; }
}
$c = new C;
var_dump($c->foo());
?>
--EXPECTF--
Warning: Can't preload unlinked class C: Parent with unresolved initializers B in %s on line %d

Warning: Can't preload class B with unresolved initializer for constant X in %s on line %d

Fatal error: Uncaught Error: Class 'C' not found in %sbug78014.php:5
Stack trace:
#0 {main}
  thrown in %sbug78014.php on line 5
