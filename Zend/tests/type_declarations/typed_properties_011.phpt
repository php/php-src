--TEST--
Test typed disallow fetch reference for init array
--FILE--
<?php
class Foo {
	public int $bar = 1;
}

$foo = new Foo();

$array = [&$foo->bar];
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must not be referenced in %s:8
Stack trace:
#0 {main}
  thrown in %s on line 8


