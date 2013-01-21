--TEST--
ZE2 Tests that a static auto-implemented setter has a protected auto-implemented variable defined and that it can be set through the accessor
--FILE--
<?php

class AccessorTest {
	public static $b {
		set;
	}

	public static function getStaticValue() {
		return self::$__b;
	}
}

$rf = new ReflectionClass('AccessorTest');
foreach($rf->getProperties(ReflectionProperty::IS_PROTECTED) as $rfp) {
	if($rfp->getName() == '__b') {
		if($rfp->isProtected() && $rfp->isStatic())
			echo "Protected static property: \$".$rfp->getName()." exists.\n";
	}
}

echo "AccessorTest::\$b: ".AccessorTest::getStaticValue()."\n";
AccessorTest::$b = 10;
echo "AccessorTest::\$b = 10;\n";
echo "AccessorTest::\$b: ".AccessorTest::getStaticValue()."\n";
echo "Done\n";
	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Protected static property: $__b exists.
AccessorTest::$b: 
AccessorTest::$b = 10;
AccessorTest::$b: 10
Done
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d