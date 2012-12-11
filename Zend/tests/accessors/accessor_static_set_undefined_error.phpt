--TEST--
ZE2 Tests that an undefined static setter emits an error
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
	}
}

AccessorTest::$b = 12;
?>
--EXPECTF--
Fatal error: Cannot set property AccessorTest::$b, no setter defined. in %s on line %d