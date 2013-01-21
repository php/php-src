--TEST--
ZE2 Tests that a static defined setter has it's code called.
--FILE--
<?php

class AccessorTest {
	public static $b {
		set {
			echo "set AccessorTest::\$b Called \$value = {$value}.\n";
		}
	}
}

echo AccessorTest::$b;
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Fatal error: Cannot get property AccessorTest::$b, no getter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d