--TEST--
ZE2 Tests that access to a static undefined self getter produces compile error (pass_two() functionality)
--FILE--
<?php

class AccessorTest {
	public static $a {
		set;
	}
	public static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return self::$a;
		}
	}
}

echo AccessorTest::$b;
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Fatal error: Cannot get property self::$a, no getter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d