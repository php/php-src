--TEST--
Trying to add an alias to a trait method where there is another with same name
--FILE--
<?php

trait foo {
	public function test() { return 3; }
}

trait baz {
	public function test() { return 4; }
}

class bar {
	use foo, baz {
		baz::test as zzz;
	}
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Failed to add aliased trait method (zzz) to the trait table. There is probably already a trait method with same name in %s on line %d
