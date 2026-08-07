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
            echo $e::class, ': ', $e->getMessage(), "\n";
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
$staticMethod = ReflectionMethod::createFromMethodName('TestClass::staticMethod');
$privateMethod = new ReflectionMethod("TestClass", "privateMethod");

echo "\nNon-instance:\n";
try {
    var_dump($foo->invokeArgs(new stdClass(), array()));
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\nStatic method:\n";

var_dump($staticMethod->invokeArgs(null, array()));

echo "\nPrivate method:\n";
var_dump($privateMethod->invokeArgs($testClassInstance, array()));

echo "\nAbstract method:\n";
$abstractMethod = new ReflectionMethod("AbstractClass", "foo");
try {
    $abstractMethod->invokeArgs($testClassInstance, array());
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $abstractMethod->invokeArgs(true);
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Non-instance:
ReflectionException: Given object is not an instance of the class this method was declared in

Static method:
Called staticMethod()
Error: Using $this when not in object context
NULL

Private method:
Called privateMethod()
NULL

Abstract method:
ReflectionException: Trying to invoke abstract method AbstractClass::foo()
ReflectionException: Trying to invoke abstract method AbstractClass::foo()
