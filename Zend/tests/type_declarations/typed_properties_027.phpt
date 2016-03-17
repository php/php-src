--TEST--
Test typed properties int must be allowed to widen to float only at runtime
--FILE--
<?php
class Foo {

	public int $bar = 1.1;
}
?>
--EXPECTF--
Fatal error: Default value for properties with integer type can only be integer in %s on line 4







