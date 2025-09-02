--TEST--
ZE2 Late Static Binding properties and methods declared as public and overridden as public.
--FILE--
<?php
class TestClass {
    public static $staticVar;

    public static function staticFunction() {
        return 'TestClassFunction';
    }

    public static function testStaticVar() {
        TestClass::$staticVar = 'TestClassStatic';
        ChildClass1::$staticVar = 'ChildClassStatic';
        return static::$staticVar;
    }

    public static function testStaticFunction() {
        return static::staticFunction();
    }
}

class ChildClass1 extends TestClass {
    public static $staticVar;

    public static function staticFunction() {
        return 'ChildClassFunction';
    }
}

class ChildClass2 extends TestClass {}

echo TestClass::testStaticVar() . "\n";
echo TestClass::testStaticFunction() . "\n";

echo ChildClass1::testStaticVar() . "\n";
echo ChildClass1::testStaticFunction() . "\n";

echo ChildClass2::testStaticVar() . "\n";
echo ChildClass2::testStaticFunction() . "\n";
?>
--EXPECT--
TestClassStatic
TestClassFunction
ChildClassStatic
ChildClassFunction
TestClassStatic
TestClassFunction
