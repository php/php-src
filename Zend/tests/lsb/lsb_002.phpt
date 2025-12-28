--TEST--
ZE2 Late Static Binding in an instance function
--FILE--
<?php

class TestClass {
    protected static $staticVar = 'TestClassStatic';
    const CLASS_CONST = 'TestClassConst';

    protected static function staticFunction() {
        return 'TestClassFunction';
    }

    public function testStaticVar() {
        return static::$staticVar;
    }

    public function testClassConst() {
        return static::CLASS_CONST;
    }

    public function testStaticFunction() {
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

$testClass = new TestClass();
$childClass1 = new ChildClass1();
$childClass2 = new ChildClass2();


echo $testClass->testStaticVar() . "\n";
echo $testClass->testClassConst() . "\n";
echo $testClass->testStaticFunction() . "\n";

echo $childClass1->testStaticVar() . "\n";
echo $childClass1->testClassConst() . "\n";
echo $childClass1->testStaticFunction() . "\n";

echo $childClass2->testStaticVar() . "\n";
echo $childClass2->testClassConst() . "\n";
echo $childClass2->testStaticFunction() . "\n";
?>
--EXPECT--
TestClassStatic
TestClassConst
TestClassFunction
ChildClassStatic
ChildClassConst
ChildClassFunction
TestClassStatic
TestClassConst
TestClassFunction
