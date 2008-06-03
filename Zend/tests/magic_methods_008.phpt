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
Fatal error: The magic method __set() must have public visibility and can not be static in %s on line %d
