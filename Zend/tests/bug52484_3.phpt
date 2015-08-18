--TEST--
Bug #52484.3 (__set() ignores setting properties with empty names)
--FILE--
<?php

class A {	
	function __get($prop) {
		var_dump($this->$prop);
	}
}

$a = new A();
$prop = null;

var_dump($a->$prop);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access empty property in %s:%d
Stack trace:
#0 %s(%d): A->__get('')
#1 {main}
  thrown in %s on line %d
