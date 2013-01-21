--TEST--
ZE2 Tests that access to a static undefined parent getter produces compile error (pass_two() functionality)
--FILE--
<?php

class ParentAccessorTest {
	public static $a {
		set {
			echo "set ParentAccessorTest::\$a Called \$value = {$value}.\n";
		}
	}
}

class AccessorTest extends ParentAccessorTest {
	public static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return parent::$a;
		}
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
Fatal error: Cannot get property parent::$a, no getter defined. in %s on line %d
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d