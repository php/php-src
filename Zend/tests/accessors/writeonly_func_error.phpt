--TEST--
ZE2 Tests that a function cannot be defined with write-only
--FILE--
<?php
class TestClass {
	public write-only function a() { }
}
?>
--EXPECTF--
Fatal error: Method TestClass::a() cannot be defined write-only, not permitted for methods. in %s on line %d
