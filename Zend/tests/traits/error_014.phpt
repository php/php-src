--TEST--
Trying to override final method
--FILE--
<?php

trait foo {
	public function test() { return 3; }
}

class baz {
	final public function test() { return 4; }
}

class bar extends baz {
	use foo { test as public; }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Cannot override final method baz::test() in %s on line %d
