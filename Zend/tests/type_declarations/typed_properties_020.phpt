--TEST--
Test typed properties binary assign op helper test
--FILE--
<?php
declare(strict_types=1);

class Foo {
	public string $bar;

	public function __construct() {
		$this->bar += 1;
	}
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be string, integer used in %s:8
Stack trace:
#0 %s(12): Foo->__construct()
#1 {main}
  thrown in %s on line 8







