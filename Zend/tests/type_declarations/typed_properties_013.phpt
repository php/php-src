--TEST--
Test typed disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
	public int $bar = 2.2;
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$bar must be integer, float used in %s on line 3



