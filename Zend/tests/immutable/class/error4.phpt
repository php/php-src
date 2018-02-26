--TEST--
Nonimmutable class cannot extend immutable class.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {}
class B extends A {}
?>
--EXPECTF--

Fatal error: Immutable class X\Y\Z\A may not be extended by non immutable class X\Y\Z\B in %s on line %d
