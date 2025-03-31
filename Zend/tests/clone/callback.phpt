--TEST--
As Callback
--FILE--
<?php

class Foo {
	private function __clone() {
		
	}

	public function clone_me() {
		return array_map(clone(...), [$this]);
	}
}

$f = new Foo();

$clone = $f->clone_me()[0];

var_dump($f !== $clone);

?>
--EXPECT--
bool(true)
