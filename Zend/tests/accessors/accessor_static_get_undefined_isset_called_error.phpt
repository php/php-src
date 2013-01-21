--TEST--
ZE2 Tests that an undefined static getter produces an error when attempting to isset()
--FILE--
<?php

class AccessorTest {
	public static $b {
		set;
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Fatal error: Cannot isset property AccessorTest::$b, no getter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d