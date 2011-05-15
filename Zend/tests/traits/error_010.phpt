--TEST--
Trying to exclude trait method multiple times
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
	use foo, c { c::test insteadof foo, b; }
	use foo, c { c::test insteadof foo, b; }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Failed to evaluate a trait precedence (test). Method of trait foo was defined to be excluded multiple times in %s on line %d
