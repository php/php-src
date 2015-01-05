--TEST--
Trying to access a protected trait method
--FILE--
<?php

trait foo {
	public function test() { return 3; }
}

class bar {
	use foo { test as protected; }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Call to protected method bar::test() from context '' in %s on line %d
