--TEST--
ReflectionMethod::invokeArgs()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class TestClass {
    public $prop = 2;

    public function foo() {
        echo "Called foo(), property = $this->prop\n";
        var_dump($this);
        return "Return Val";
    }

    public function willThrow() {
        throw new Exception("Called willThrow()");
    }

    public function methodWithArgs($a, $b) {
        echo "Called methodWithArgs($a, $b)\n";
    }
}


$testClassInstance = new TestClass();
$testClassInstance->prop = "Hello";

$foo = new ReflectionMethod($testClassInstance, 'foo');
$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');
$methodThatThrows = new ReflectionMethod("TestClass::willThrow");


echo "Public method:\n";

var_dump($foo->invokeArgs($testClassInstance, array()));
var_dump($foo->invokeArgs($testClassInstance, array(true)));

echo "\nMethod with args:\n";

var_dump($methodWithArgs->invokeArgs($testClassInstance, array(1, "arg2")));
var_dump($methodWithArgs->invokeArgs($testClassInstance, array(1, "arg2", 3)));

echo "\nMethod that throws an exception:\n";
try {
    $methodThatThrows->invokeArgs($testClassInstance, array());
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Public method:
Called foo(), property = Hello
object(TestClass)#%d (1) {
  ["prop"]=>
  string(5) "Hello"
}
string(10) "Return Val"
Called foo(), property = Hello
object(TestClass)#%d (1) {
  ["prop"]=>
  string(5) "Hello"
}
string(10) "Return Val"

Method with args:
Called methodWithArgs(1, arg2)
NULL
Called methodWithArgs(1, arg2)
NULL

Method that throws an exception:
string(18) "Called willThrow()"
