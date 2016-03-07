--TEST--
Test typed disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
	public stdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Type of Foo::$bar must be stdClass, null used in %s on line 3



