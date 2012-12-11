--TEST--
ZE2 Tests that a static undefined isset is equivalent to an auto-implemented one (returns true if non-NULL)
--FILE--
<?php

class AccessorTest {
	public static $b {
		get;
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
?>
--EXPECTF--
isset(AccessorTest::$b) = 0
AccessorTest::setStaticValue(10);
isset(AccessorTest::$b) = 1
AccessorTest::setStaticValue(NULL);
isset(AccessorTest::$b) = 0
Done