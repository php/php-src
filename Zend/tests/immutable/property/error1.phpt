--TEST--
Multiple immutable modifiers not allowed on properties.
--FILE--
<?php
namespace X\Y\Z;
class A {
	public immutable immutable $a;
}
?>
--EXPECTF--

Fatal error: Multiple immutable modifiers are not allowed in %s on line %d
