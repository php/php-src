--TEST--
Testing trait collisions
--FILE--
<?php

trait foo {
	public function test() { return 3; }
}
trait c {
	public function test() { return 2; }
}

trait b {
	public function test() { return 1; }
}

class bar {
	use foo, c, b;
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Trait method test has not been applied, because there are collisions with other trait methods on bar in %s on line %d
