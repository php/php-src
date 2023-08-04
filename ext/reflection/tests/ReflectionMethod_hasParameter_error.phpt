--TEST--
ReflectionMethod::hasParameter()
--FILE--
<?php
class C {
    public function foo(string $bar) {}
}

$method = new ReflectionMethod('WeakReference', 'create');

try {
    var_dump($method->hasParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

$method = new ReflectionMethod('C', 'foo');

try {
    var_dump($method->hasParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}
?>
--EXPECT--
ReflectionFunctionAbstract::hasParameter(): Argument #1 ($parameter) must be greater than or equal to 0
ReflectionFunctionAbstract::hasParameter(): Argument #1 ($parameter) must be greater than or equal to 0
