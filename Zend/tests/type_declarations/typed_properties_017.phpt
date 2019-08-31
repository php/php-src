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
Fatal error: Property Foo::$int cannot have type void in %s on line 3
