--TEST--
Bug #70215 (Segfault when invoke is static)
--FILE--
<?php

class A {
	public static function __invoke() {
		echo __CLASS__;
	}
}

class B extends A { }

$b = new B;

$b();

?>
--EXPECTF--
Warning: The magic method __invoke() must have public visibility and cannot be static in %s on line %d
A
