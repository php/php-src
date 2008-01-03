--TEST--
ReflectionMethod::returnsReference()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class TestClass {
    public function &foo() {
    }

    private function bar() {
    }
}

$methodInfo = new ReflectionMethod('TestClass::foo');
var_dump($methodInfo->returnsReference());

$methodInfo = new ReflectionMethod('TestClass::bar');
var_dump($methodInfo->returnsReference());

?>
--EXPECT--
bool(true)
bool(false)
