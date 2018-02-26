--TEST--
Mutation of properties from immutable classes is not possible after construction.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
}

$a = new A(1);
$a->x = 2;
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot modify state of immutable object after constructor in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
