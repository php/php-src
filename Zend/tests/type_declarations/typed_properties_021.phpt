--TEST--
Test typed properties delay type check on constant
--FILE--
<?php
class Foo {
	public int $bar = BAR::BAZ;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Class 'BAR' not found in %s on line 6






