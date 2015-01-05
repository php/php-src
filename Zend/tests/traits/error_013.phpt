--TEST--
Trying to use static as method modifier
--FILE--
<?php

trait foo {
	public function test() { return 3; }
}

class bar {
	use foo { test as static; }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Cannot use 'static' as method modifier in %s on line %d
