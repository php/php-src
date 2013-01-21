--TEST--
ZE2 Tests that an undefined static getter produces an error indicating it's non-existence
--FILE--
<?php

class AccessorTest {
	public static $b {
		set;
	}
}

echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Fatal error: Cannot get property AccessorTest::$b, no getter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d