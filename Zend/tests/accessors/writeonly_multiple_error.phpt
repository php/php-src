--TEST--
ZE2 Tests that a property cannot be defined with write-only multiple times
--FILE--
<?php
class TestClass {
	public write-only write-only $a;
}
?>
--EXPECTF--
Fatal error: Multiple write-only modifiers are not allowed in %s on line %d
