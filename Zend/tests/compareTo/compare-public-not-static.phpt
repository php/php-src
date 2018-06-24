--TEST--
__compareTo: Magic method must be public and non-static
--FILE--
<?php

class A {
	private function __compareTo() {}
}

class B {
	protected function __compareTo() {}
}

class C {
	static public function __compareTo() {}
}

?>
--EXPECTF--
Warning: The magic method __compareTo() must have public visibility and cannot be static in %s on line %d

Warning: The magic method __compareTo() must have public visibility and cannot be static in %s on line %d

Warning: The magic method __compareTo() must have public visibility and cannot be static in %s on line %d
