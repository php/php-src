--TEST--
Test typed properties disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
	public array $bar = 32;
}
?>
--EXPECTF--
Fatal error: Default value for property of type array can only be an array in %s on line 3
