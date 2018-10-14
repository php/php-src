--TEST--
ZE2 Late Static Binding in a static function
--FILE--
<?php

class TestClass {
	protected static $staticVar = 'TestClassStatic';
	const CLASS_CONST = 'TestClassConst';

	protected static function staticFunction() {
		return 'TestClassFunction';
	}

	public static function testStaticVar() {
		return static::$staticVar;
	}

	public static function testClassConst() {
		return static::CLASS_CONST;
	}

	public static function testStaticFunction() {
		return static::staticFunction();
	}
}

class ChildClass1 extends TestClass {
	protected static $staticVar = 'ChildClassStatic';
	const CLASS_CONST = 'ChildClassConst';

	protected static function staticFunction() {
		return 'ChildClassFunction';
	}
}

class ChildClass2 extends TestClass {}

echo TestClass::testStaticVar() . "\n";
echo TestClass::testClassConst() . "\n";
echo TestClass::testStaticFunction() . "\n";

echo ChildClass1::testStaticVar() . "\n";
echo ChildClass1::testClassConst() . "\n";
echo ChildClass1::testStaticFunction() . "\n";

echo ChildClass2::testStaticVar() . "\n";
echo ChildClass2::testClassConst() . "\n";
echo ChildClass2::testStaticFunction() . "\n";
?>
==DONE==
--EXPECTF--
TestClassStatic
TestClassConst
TestClassFunction
ChildClassStatic
ChildClassConst
ChildClassFunction
TestClassStatic
TestClassConst
TestClassFunction
==DONE==
