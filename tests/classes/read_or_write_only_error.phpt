--TEST--
ZE2 Tests that that read-only and write-only are mutually exclusive
--FILE--
<?php
class TestClass {
	public read-only write-only $a {

	}
}
?>
--EXPECTF--
Fatal error: read-only and write-only modifiers are mutually exclusive in %s on line %d
