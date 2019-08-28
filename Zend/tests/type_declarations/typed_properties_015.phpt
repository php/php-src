--TEST--
Test typed properties disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
	public StdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Default value for property of type StdClass may not be null. Use the nullable type ?StdClass to allow null default value in %s on line %d
