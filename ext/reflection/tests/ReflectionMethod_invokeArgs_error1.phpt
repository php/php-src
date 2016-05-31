--TEST--
ReflectionMethod:invokeArgs() errors
--FILE--
<?php

class TestClass {

    public function methodWithArgs($a, $b) {
        echo "Called methodWithArgs($a, $b)\n";
    }
}

abstract class AbstractClass {
    abstract function foo();
}

$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');

$testClassInstance = new TestClass();

echo "\nMethod with args:\n";
var_dump($methodWithArgs->invokeArgs($testClassInstance, array())); 

?>
--EXPECTF--
Method with args:

Fatal error: Uncaught Error: Too few arguments to function TestClass::methodWithArgs(), 0 passed and exactly 2 expected in %sReflectionMethod_invokeArgs_error1.php:5
Stack trace:
#0 [internal function]: TestClass->methodWithArgs()
#1 %sReflectionMethod_invokeArgs_error1.php(19): ReflectionMethod->invokeArgs(Object(TestClass), Array)
#2 {main}
  thrown in %sReflectionMethod_invokeArgs_error1.php on line 5
