--TEST--
Argument/return types must be available for preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_variance_ind.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
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
--EXPECTF--
Warning: Can't preload unlinked class H: Could not check compatibility between H::method($a): L and G::method($a): K, because class L is not available in %spreload_variance.inc on line 43
