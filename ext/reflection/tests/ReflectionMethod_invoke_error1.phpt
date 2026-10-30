--TEST--
ReflectionMethod::invoke() errors
--FILE--
<?php

class TestClass {
    public $prop = 2;

    public function foo() {
        echo "Called foo(), property = $this->prop\n";
        var_dump($this);
        return "Return Val";
    }

    private static function privateMethod() {
        echo "Called privateMethod()\n";
    }
}

abstract class AbstractClass {
    abstract function foo();
}

$foo = new ReflectionMethod('TestClass', 'foo');
$privateMethod = ReflectionMethod::createFromMethodName("TestClass::privateMethod");

$testClassInstance = new TestClass();
$testClassInstance->prop = "Hello";

echo "invoke() on a non-object:\n";
try {
    var_dump($foo->invoke(true));
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

echo "\ninvoke() on a non-instance:\n";
try {
    var_dump($foo->invoke(new stdClass()));
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

echo "\nPrivate method:\n";
var_dump($privateMethod->invoke($testClassInstance));

echo "\nAbstract method:\n";
$abstractMethod = ReflectionMethod::createFromMethodName("AbstractClass::foo");
try {
    $abstractMethod->invoke(true);
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
invoke() on a non-object:
string(85) "ReflectionMethod::invoke(): Argument #1 ($object) must be of type ?object, true given"

invoke() on a non-instance:
string(72) "Given object is not an instance of the class this method was declared in"

Private method:
Called privateMethod()
NULL

Abstract method:
string(53) "Trying to invoke abstract method AbstractClass::foo()"
