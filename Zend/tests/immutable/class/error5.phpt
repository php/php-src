--TEST--
Immutable classes are not cloneable.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {}
$a = new A;
clone $a;
?>
--EXPECTF--

Fatal error: Uncaught Error: Cloning of immutable class X\Y\Z\A is not permitted in %simmutable_modifier%sclass%serror5.php:7
Stack trace:
#0 {main}
  thrown in %simmutable%sclass%serror5.php on line 7
