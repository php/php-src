--TEST--
ZE2 Tests that a static auto-implemented getter has a protected auto-implemented variable defined and that it can be retrieved through the accessor
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

$rf = new ReflectionClass('AccessorTest');
foreach($rf->getProperties(ReflectionProperty::IS_PROTECTED) as $rfp) {
	if($rfp->getName() == '__b') {
		if($rfp->isProtected() && $rfp->isStatic())
			echo "Protected static property: \$".$rfp->getName()." exists.\n";
	}
}

echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
AccessorTest::setStaticValue(5);
echo "AccessorTest::setStaticValue(5);\n";
echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
echo "Done\n";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Protected static property: $__b exists.
AccessorTest::$b: 
AccessorTest::setStaticValue(5);
AccessorTest::$b: 5
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d