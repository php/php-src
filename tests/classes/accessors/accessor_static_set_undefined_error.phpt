--TEST--
ZE2 Tests that an undefined static setter emits an error
--FILE--
<?php

class AccessorTest {
	public static $b {
	}
}

AccessorTest::$b = 12;
?>
--EXPECTF--
Fatal error: Access to undeclared static property: AccessorTest::$b in %s on line %d