--TEST--
Test typed properties int must be allowed to widen to float only at runtime
--FILE--
<?php
class Foo {

	public int $bar = 1.1;
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$bar must be integer, float used in %s on line 4






