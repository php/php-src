--TEST--
Testing __set implementation with wrong declaration
--FILE--
<?php

abstract class b {
	abstract function __set($a, $b);
}

class a extends b {
	private function __set($a, $b) {
	}
}

?>
--EXPECTF--
Warning: The magic method __set() must have public visibility and cannot be static in %s on line %d

Fatal error: Access level to a::__set() must be public (as in class b) in %s on line %d
