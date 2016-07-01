--TEST--
ReflectionMethod::invoke()
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

    public static function staticMethod() {
        echo "Called staticMethod()\n";
        try {
	        var_dump($this);
		} catch (Throwable $e) {
			echo "Exception: " . $e->getMessage() . "\n";
		}
    }

    private static function privateMethod() {
        echo "Called privateMethod()\n";
    }
}

abstract class AbstractClass {
    abstract function foo();
}

$foo = new ReflectionMethod('TestClass', 'foo');
$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');
$staticMethod = new ReflectionMethod('TestClass::staticMethod');
$privateMethod = new ReflectionMethod("TestClass::privateMethod");
$methodThatThrows = new ReflectionMethod("TestClass::willThrow");

$testClassInstance = new TestClass();
$testClassInstance->prop = "Hello";

echo "Public method:\n";

var_dump($foo->invoke($testClassInstance));

var_dump($foo->invoke($testClassInstance, true));

echo "\nMethod with args:\n";

var_dump($methodWithArgs->invoke($testClassInstance, 1, "arg2"));
var_dump($methodWithArgs->invoke($testClassInstance, 1, "arg2", 3));

echo "\nStatic method:\n";

var_dump($staticMethod->invoke());
var_dump($staticMethod->invoke(true));
var_dump($staticMethod->invoke(new stdClass()));

echo "\nMethod that throws an exception:\n";
try {
	var_dump($methodThatThrows->invoke($testClassInstance));
} catch (Exception $exc) {
	var_dump($exc->getMessage());
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

Static method:

Warning: ReflectionMethod::invoke() expects at least 1 parameter, 0 given in %s on line %d
NULL
Called staticMethod()
Exception: Using $this when not in object context
NULL
Called staticMethod()
Exception: Using $this when not in object context
NULL

Method that throws an exception:
string(18) "Called willThrow()"
