--TEST--
ZE2 Tests that a property cannot be defined with read-only multiple times
--FILE--
<?php
class TestClass {
	public read-only read-only $a;
}
?>
--EXPECTF--
Fatal error: Multiple read-only modifiers are not allowed in %s on line %d
