--TEST--
ZE2 Tests that a static defined getter has it's code called and the return value is represented as the value of the original accessor reference
--FILE--
<?php

class AccessorTest {
	public static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return 5;
		}
	}
}

echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
echo "Done\n";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
get AccessorTest::$b Called.
AccessorTest::$b: 5
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d