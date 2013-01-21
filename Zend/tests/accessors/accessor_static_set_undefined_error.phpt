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
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Fatal error: Cannot set property AccessorTest::$b, no setter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d