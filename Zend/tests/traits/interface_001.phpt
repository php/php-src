--TEST--
Using traits to implement interface
--FILE--
<?php

trait foo {
	public function abc() {
	}
}

interface baz {
	public function abc();
}

class bar implements baz {
	use foo;

}

new bar;
print "OK\n";

?>
--EXPECT--
OK
