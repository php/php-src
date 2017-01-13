--TEST--
Bug #71978 (Existence of return type hint affects other compatibility rules)
--FILE--
<?php
class A {
	function foo(int $a) {}
}
class B extends A {
	function foo(string $a) {}
}
class A1 {
	function foo(int $a): int {}
}
class B1 extends A1 {
	function foo(string $a): int {}
}
?>
--EXPECTF--
Warning: Declaration of B::foo(string $a) should be compatible with A::foo(int $a) in %s on line %d

Warning: Declaration of B1::foo(string $a): int should be compatible with A1::foo(int $a): int in %s on line %d
