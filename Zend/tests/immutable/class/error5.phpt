--TEST--
Can not assign array to immutable property.
--DESCRIPTION--

--FILE--
<?php
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
}

new A([0, 1, 2]);
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot assign array to immutable property: x in %s:%d
Stack trace:
#0 %s(%d): A->__construct(Array)
#1 {main}
  thrown in %s on line %d
