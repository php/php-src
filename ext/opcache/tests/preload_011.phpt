--TEST--
Argument/return types must be available for preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_variance_ind.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
interface K {}
interface L extends K {}
require __DIR__ . '/preload_variance.inc';

$a = new A;
$b = new B;
$d = new D;
$f = new F;
$g = new G;

?>
===DONE===
--EXPECTF--
Warning: Can't preload unlinked class H: Unknown type dependencies in %s on line %d

Warning: Can't preload unlinked class B: Unknown type dependencies in %s on line %d

Warning: Can't preload unlinked class A: Unknown type dependencies in %s on line %d
===DONE===
