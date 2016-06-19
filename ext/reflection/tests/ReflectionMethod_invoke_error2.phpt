--TEST--
ReflectionMethod::invoke() further errors
--FILE--
<?php

class TestClass {

    public function methodWithArgs($a, $b) {
        echo "Called methodWithArgs($a, $b)\n";
    }
}

$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');

$testClassInstance = new TestClass();

echo "\nMethod with args:\n";
var_dump($methodWithArgs->invoke($testClassInstance));

?>
--EXPECTF--
Method with args:

Fatal error: Uncaught Error: Too few arguments to function TestClass::methodWithArgs(), 0 passed and exactly 2 expected in %sReflectionMethod_invoke_error2.php:5
Stack trace:
#0 [internal function]: TestClass->methodWithArgs()
#1 %sReflectionMethod_invoke_error2.php(15): ReflectionMethod->invoke(Object(TestClass))
#2 {main}
  thrown in %sReflectionMethod_invoke_error2.php on line 5
