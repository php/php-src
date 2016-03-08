--TEST--
Test typed disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
	public stdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$bar must be an instance of stdClass, null used in %s on line 3



