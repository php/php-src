--TEST--
Overwritten protected asymmetric property with private asymmetric property
--FILE--
<?php

class A {
	public protected(set) string $foo;
}

class B extends A {
	public private(set) string $foo;
}

?>
--EXPECTF--
Fatal error: Set access level of B::$foo must be protected(set) (as in class A) or weaker in %s on line %d
