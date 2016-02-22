--TEST--
Defining trait that implements an interface with abstract methods and via another trait
--FILE--
<?php

interface baz {
	public function abc();
}

trait foo implements baz {
	abstract public function abc();
}

trait quux {
	function abc() {
	}
}

trait bar implements baz {
	use quux;
}

print "OK\n";

?>
--EXPECT--
OK
