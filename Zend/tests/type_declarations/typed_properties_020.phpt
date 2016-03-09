--TEST--
Test typed properties do not allow coerced values to be used
--FILE--
<?php
class Foo {
	public string $bar;

	public function __construct() {
		$this->bar += 1;

		var_dump($this->bar);
	}
}

$foo = new Foo();

var_dump($foo->bar);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be string, integer used in %s:8
Stack trace:
#0 %s(12): Foo->__construct()
#1 {main}
  thrown in %s on line 8





