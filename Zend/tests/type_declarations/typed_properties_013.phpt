--TEST--
Test typed disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
	public int $bar = "string";
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$bar must be integer, string used in %s on line 3



