--TEST--
Nonimmutable class cannot extend immutable class.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {}
class B extends A {}
?>
--EXPECTF--

Fatal error: Immutable class X\Y\Z\B cannot extend immutable class X\Y\Z\A in %serror4.php on line 6
