--TEST--
Can not assign resource to immutable property.
--FILE--
<?php
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
}

$x = new A(fopen(__DIR__.'/error8.phpt', 'r'));
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot assign resource to immutable property: x in %s:%d
Stack trace:
#0 %s(%d): A->__construct(Resource id #5)
#1 {main}
  thrown in %s on line %d
