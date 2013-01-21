--TEST--
ZE2 Tests that a static defined unset will have the unset code called
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
		set;
		isset;
		unset {
			echo get_called_class()."::\$b Unsetter Called.\n";
			self::$b = NULL;
		}
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
AccessorTest::$b Unsetter Called.
unset(AccessorTest::$b);
isset(AccessorTest::$b) = 0
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d