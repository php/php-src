--TEST--
Overwritten public property with protected asymmetric property
--FILE--
<?php

class A {
	public string $foo;
}

class B extends A {
	public protected(set) string $foo;
}

?>
--EXPECTF--
Fatal error: Set access level of B::$foo must be omitted (as in class A) in %s on line %d
