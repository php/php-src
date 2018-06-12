--TEST--
Test typed properties disallow void
--FILE--
<?php
class Foo {
	public void $int;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Typed property Foo::$int cannot have type void in %s on line 3
