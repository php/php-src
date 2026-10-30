--TEST--
ReflectionMethod::returnsReference()
--FILE--
<?php

class TestClass {
    public function &foo() {
    }

    private function bar() {
    }
}

$methodInfo = ReflectionMethod::createFromMethodName('TestClass::foo');
var_dump($methodInfo->returnsReference());

$methodInfo = ReflectionMethod::createFromMethodName('TestClass::bar');
var_dump($methodInfo->returnsReference());

?>
--EXPECT--
bool(true)
bool(false)
