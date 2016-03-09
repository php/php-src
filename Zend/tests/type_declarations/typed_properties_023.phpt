--TEST--
Test typed properties disallow static
--FILE--
<?php
class Foo {
	public static int $thing;
}

$foo = new Foo();

var_dump($foo->bar);
?>
--EXPECTF--
Fatal error: Typed property Foo::$thing must not be static in %s on line 3







