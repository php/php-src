--TEST--
Test typed properties disallow static
--FILE--
<?php
class Foo {
	public static int $thing;
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$thing must not be static in %s on line 3







