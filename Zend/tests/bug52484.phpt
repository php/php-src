--TEST--
Bug #52484 (__set() ignores setting properties with empty names)
--FILE--
<?php

class A {	
	function __unset($prop) {
		unset($this->$prop);
	}
}

$a = new A();
$prop = null;

unset($a->$prop);

?>
--EXPECTF--
Fatal error: Cannot access empty property in %s on line %d
