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
--EXPECT--

Fatal error: Uncaught Error: Cannot assign array to immutable property: x in %simmutable%sclass%serror5.php:5
Stack trace:
#0 %simmutable%sclass%serror5.php(9): A->__construct(Array)
#1 {main}
  thrown in %simmutable%sclass%serror5.php on line 5
