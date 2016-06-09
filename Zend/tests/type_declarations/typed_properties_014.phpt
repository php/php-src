--TEST--
Test typed properties disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
	public array $bar = null;
}
?>
--EXPECTF--
Fatal error: Default value for properties with array type can only be an array in %s on line 3



