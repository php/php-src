--TEST--
Nullable return type inheritance rules (nullable and non-nullable)
--FILE--
<?php
class A {
	function foo(): ?int {}
}
class B extends A {
	function foo(): int {}
}
?>
DONE
--EXPECT--
DONE
