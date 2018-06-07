--TEST--
Test typed properties unset __get magical magic
--FILE--
<?php
class Foo {
	public int $bar;

	public function __get($name) {
		return "violate";
	}
}

$foo = new Foo;

$foo->bar = "1"; # ok

unset($foo->bar); # ok

var_dump($foo->bar); # not okay, __get is nasty
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be int, string used in %s:16
Stack trace:
#0 {main}
  thrown in %s on line 16
