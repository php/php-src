--TEST--
ZE2 Tests that a static undefined unset is equivalent to an autoimplemented unset, which sets the property to a NULL value
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
		set;
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
AccessorTest::$b = 10;
echo "AccessorTest::\$b = 10;\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
unset(AccessorTest::$b);
echo "unset(AccessorTest::\$b);\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
echo "Done";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
isset(AccessorTest::$b) = 0
AccessorTest::$b = 10;
isset(AccessorTest::$b) = 1
unset(AccessorTest::$b);
isset(AccessorTest::$b) = 0
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d