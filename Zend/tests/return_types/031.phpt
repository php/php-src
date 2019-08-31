--TEST--
Nullable return type inheritance rules (non-nullable and nullable)
--FILE--
<?php
class A {
	function foo(): int {}
}
class B extends A {
	function foo(): ?int {}
}
?>
DONE
--EXPECTF--
Fatal error: Declaration of B::foo(): ?int must be compatible with A::foo(): int in %s031.php on line 6
