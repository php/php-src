--TEST--
Test typed no void
--FILE--
<?php
class Foo {
	public void $int;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Typed property Foo::$int must not be void in %s on line 3




