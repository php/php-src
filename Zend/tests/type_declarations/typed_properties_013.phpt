--TEST--
Test typed properties disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
	public int $bar = "string";
}
?>
--EXPECTF--
Fatal error: Default value for property of type int can only be int in %s on line 3
