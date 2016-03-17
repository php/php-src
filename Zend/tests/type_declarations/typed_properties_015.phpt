--TEST--
Test typed properties disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
	public stdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Default value for properties with class type are disallowed in %s on line 3




