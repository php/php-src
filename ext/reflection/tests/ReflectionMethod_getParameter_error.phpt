--TEST--
ReflectionMethod::getParameter() errors
--FILE--
<?php
$method = new ReflectionMethod('WeakReference', 'create');

try {
    var_dump($method->getParameter('Object'));
} catch (ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($method->getParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($method->getParameter(3));
} catch (ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

class C {
    public function foo(string $bar) {}
}

$method = new ReflectionMethod('C', 'foo');

try {
    var_dump($method->getParameter('Bar'));
} catch (ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($method->getParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($method->getParameter(1));
} catch (ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

$method = new ReflectionMethod('WeakReference', 'get');

try {
    var_dump($method->getParameter(1));
} catch (ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}
?>
--EXPECT--
Method WeakReference::create() has no parameter named "Object"
ReflectionFunctionAbstract::getParameter(): Argument #1 ($param) must be greater than or equal to 0
Method WeakReference::create() has no parameter at offset 3
Method C::foo() has no parameter named "Bar"
ReflectionFunctionAbstract::getParameter(): Argument #1 ($param) must be greater than or equal to 0
Method C::foo() has no parameter at offset 1
Method WeakReference::get() has no parameters
