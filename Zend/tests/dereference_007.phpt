--TEST--
Trying to write on method return
--FILE--
<?php

error_reporting(E_ALL);

class foo {
	public $x = array();
		
	public function b() {
		return $this->x;
	}
}

$foo = new foo;

$foo->b()[0] = 1;

?>
--EXPECTF--
Fatal error: Can't use method return value in write context in %s on line %d
