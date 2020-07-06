--TEST--
Typed class constants (inheritance; missing type in parent)
--FILE--
<?php
class A {
	public const A = 1;
}

class B extends A {
	public const int A = 0;
}
?>
--EXPECTF--
Fatal error: Type of B::A must not be defined (as in class A) in %s on line %d
