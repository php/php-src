--TEST--
ZE2 Tests that a property cannot be defined with read-only
--FILE--
<?php
class TestClass {
	public read-only $a;
}
?>
--EXPECTF--
Fatal error: Properties cannot be declared read-only in %s on line %d
