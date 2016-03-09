--TEST--
Test typed properties binary assign op helper test
--FILE--
<?php
class Foo {
	public string $bar;

	public function __construct() {
		$this->bar += 1;
	}
}

$foo = new Foo();

var_dump($foo->bar);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be string, integer used in %s:6
Stack trace:
#0 %s(10): Foo->__construct()
#1 {main}
  thrown in %s on line 6






