--TEST--
ZE2 Tests that a static autoimplemented unset will set the property to a NULL value
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
		set;
		isset;
		unset;
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
?>
--EXPECTF--
isset(AccessorTest::$b) = 0
AccessorTest::$b = 10;
isset(AccessorTest::$b) = 1
unset(AccessorTest::$b);
isset(AccessorTest::$b) = 0
Done