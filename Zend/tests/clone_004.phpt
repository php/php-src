--TEST--
Testing usage of object as array on clone statement
--FILE--
<?php

class foo {
	public function __get($a) {
		return new $this;
	}
}

$c = new foo;

$a = clone $c->b[1];

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type foo as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
