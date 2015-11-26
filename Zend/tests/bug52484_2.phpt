--TEST--
Bug #52484.2 (__set() ignores setting properties with empty names)
--FILE--
<?php

class A {
	function __set($prop, $val) {
		$this->$prop = $val;
	}
}

$a = new A();
$prop = null;

$a->$prop = 2;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access empty property in %s:%d
Stack trace:
#0 %s(%d): A->__set('', 2)
#1 {main}
  thrown in %s on line %d
