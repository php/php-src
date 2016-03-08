--TEST--
Test typed disallow unset
--FILE--
<?php
class Foo {
	public int $bar;
}

$foo = new Foo();
unset($foo->bar);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must not be unset in %s:7
Stack trace:
#0 {main}
  thrown in %s on line 7


