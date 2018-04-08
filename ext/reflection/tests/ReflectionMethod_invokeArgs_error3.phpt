--TEST--
ReflectionMethod::invokeArgs() further errors
--FILE--
<?php

class TestClass {
    public $prop = 2;

    public function foo() {
        echo "Called foo(), property = $this->prop\n";
        var_dump($this);
        return "Return Val";
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

$testClassInstance = new TestClass();
$testClassInstance->prop = "Hello";

$foo = new ReflectionMethod($testClassInstance, 'foo');
$staticMethod = new ReflectionMethod('TestClass::staticMethod');
$privateMethod = new ReflectionMethod("TestClass::privateMethod");

echo "Wrong number of parameters:\n";
var_dump($foo->invokeArgs());
var_dump($foo->invokeArgs(true));

echo "\nNon-instance:\n";
try {
    var_dump($foo->invokeArgs(new stdClass(), array()));
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

echo "\nNon-object:\n";
var_dump($foo->invokeArgs(true, array()));

echo "\nStatic method:\n";

var_dump($staticMethod->invokeArgs());
var_dump($staticMethod->invokeArgs(true));
var_dump($staticMethod->invokeArgs(true, array()));
var_dump($staticMethod->invokeArgs(null, array()));

echo "\nPrivate method:\n";
try {
    var_dump($privateMethod->invokeArgs($testClassInstance, array()));
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

echo "\nAbstract method:\n";
$abstractMethod = new ReflectionMethod("AbstractClass::foo");
try {
    $abstractMethod->invokeArgs($testClassInstance, array());
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}
try {
    $abstractMethod->invokeArgs(true);
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Wrong number of parameters:

Warning: ReflectionMethod::invokeArgs() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ReflectionMethod::invokeArgs() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Non-instance:
string(72) "Given object is not an instance of the class this method was declared in"

Non-object:

Warning: ReflectionMethod::invokeArgs() expects parameter 1 to be object, bool given in %s on line %d
NULL

Static method:

Warning: ReflectionMethod::invokeArgs() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ReflectionMethod::invokeArgs() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::invokeArgs() expects parameter 1 to be object, bool given in %s on line %d
NULL
Called staticMethod()
Exception: Using $this when not in object context
NULL

Private method:
string(86) "Trying to invoke private method TestClass::privateMethod() from scope ReflectionMethod"

Abstract method:
string(53) "Trying to invoke abstract method AbstractClass::foo()"
string(53) "Trying to invoke abstract method AbstractClass::foo()"
