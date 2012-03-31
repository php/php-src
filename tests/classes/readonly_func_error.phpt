--TEST--
ZE2 Tests that a function cannot be defined with read-only
--FILE--
<?php
class TestClass {
	public read-only function a() { }
}
?>
--EXPECTF--
Fatal error: Method TestClass::a() cannot be defined read-only, not permitted for methods. in %s on line %d
