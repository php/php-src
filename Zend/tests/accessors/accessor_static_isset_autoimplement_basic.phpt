--TEST--
ZE2 Tests that a static auto-implemented isset is equivalent to checking the getter for a NULL/non-NULL value
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
		isset;
	}
	
	public static function setStaticValue($value) {
		self::$__b = $value;
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
AccessorTest::setStaticValue(10);
echo "AccessorTest::setStaticValue(10);\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
AccessorTest::setStaticValue(NULL);
echo "AccessorTest::setStaticValue(NULL);\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
echo "Done";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
isset(AccessorTest::$b) = 0
AccessorTest::setStaticValue(10);
isset(AccessorTest::$b) = 1
AccessorTest::setStaticValue(NULL);
isset(AccessorTest::$b) = 0
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d