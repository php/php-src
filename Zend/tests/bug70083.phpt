--TEST--
Bug #70083 (Use after free with assign by ref to overloaded objects)
--FILE--
<?php

class foo {
	private $var;
	function __get($e) {
		return $this;
	}
}

function &noref() { $foo = 1; return $foo; }

$foo = new foo;
$foo->i = &noref();
var_dump($foo);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
