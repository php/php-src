--TEST--
Test typed properties unset leaves properties in an uninitialized state
--FILE--
<?php
class Foo {
	public int $bar;

	public function __get($name) {
		var_dump($name);
	}
}

$foo = new Foo();

unset($foo->bar);

var_dump($foo->bar);
?>
--EXPECTF--
string(3) "bar"

Fatal error: Uncaught TypeError: Typed property Foo::$bar must not be accessed before initialization in %s:14
Stack trace:
#0 {main}
  thrown in %s on line 14


