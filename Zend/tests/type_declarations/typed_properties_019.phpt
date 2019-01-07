--TEST--
Test typed properties int must not be allowed to overflow
--FILE--
<?php
class Foo {
	public int $bar = PHP_INT_MAX;

	public function inc() {
		return ++$this->bar;
	}
}

$foo = new Foo();

var_dump($foo->inc());
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be int, float used in %s:6
Stack trace:
#0 %s(12): Foo->inc()
#1 {main}
  thrown in %s on line 6
