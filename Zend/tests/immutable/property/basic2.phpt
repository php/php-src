--TEST--
Mutation of immutable property is possible during construction.
--DESCRIPTION--
This test performs multiple write operations on the same property to illustrate
that the state of the immutable property can be mutated at will during
construction.
--FILE--
<?php
class A {
	public immutable $x;
	public function __construct($x, $y) {
		$this->x = $x;
		if ($y) {
			$this->x *= 2;
		}
	}
}
var_dump((new A(1, true))->x);
?>
--EXPECT--

int(2)
