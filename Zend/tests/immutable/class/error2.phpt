--TEST--
Mutation of properties inside immutable classes is not possible after construction.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
	public function setX($x) {
		$this->x = $x;
	}
}
(new A(1))->setX(2);
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot modify state of immutable object after constructor in %s:%d
Stack trace:
#0 %s(%d): X\Y\Z\A->setX(2)
#1 {main}
  thrown in %s on line %d
