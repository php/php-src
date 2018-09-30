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
Fatal error: Uncaught Error: Class 'BAR' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
