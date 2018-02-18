--TEST--
Ensure that multiple immutable property modifiers result in a fatal error.
--FILE--
<?php
namespace X\Y\Z;
class A {
	public immutable immutable $a;
}
?>
--EXPECTF--

Fatal error: Multiple immutable modifiers are not allowed in %simmutable_modifier%sclass%serror1.php on line 3
