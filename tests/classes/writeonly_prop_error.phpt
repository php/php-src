--TEST--
ZE2 Tests that a property cannot be defined with write-only
--FILE--
<?php
class TestClass {
	public write-only $a;
}
?>
--EXPECTF--
Fatal error: Properties cannot be declared write-only in %s on line %d
