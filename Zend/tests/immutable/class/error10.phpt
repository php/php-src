--TEST--
Immutable objects are compared by their values.
--DESCRIPTION--

--FILE--
<?php
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
}

$x = new A(1);
$y = new A(2);

var_dump($x === $y);
?>
--EXPECT--
bool(true)
