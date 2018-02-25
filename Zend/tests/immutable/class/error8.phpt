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

$x = new A(fopen(__DIR__.'/error5.out', 'r'));
?>
--EXPECT--

Fatal error: Uncaught Error: Cannot assign resource to immutable property: x in %serror8.php:5
Stack trace:
#0 %serror8.php(9): A->__construct(Resource id #5)
#1 {main}
  thrown in %serror8.php on line 5
