--TEST--
Mutation is possible during construction of immutable classes.
--DESCRIPTION--
This test performs multiple write operations on the same property to illustrate
that the state of the immutable object can be mutated at will during
construction.
--FILE--
<?php
immutable abstract class A {
	public $x;
	public function __construct($x, $y) {
		$this->x = $x;
		if ($y) {
			$this->x *= 2;
		}
	}
}
immutable class B extends A {}
immutable final class C extends B {}
var_dump((new C(1, true))->x);
?>
--EXPECT--

int(2)
