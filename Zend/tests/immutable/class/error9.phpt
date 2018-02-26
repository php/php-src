--TEST--
Can not assign array as default value to immutable property.
--DESCRIPTION--

--FILE--
<?php
immutable class A {
	public $x = [];
	public function __construct($x) {
		$this->x = $x;
	}
}

new A([0, 1, 2]);
?>
--EXPECTF--

Fatal error: Cannot assign array to immutable property x in %s on line %d
