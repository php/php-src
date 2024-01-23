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
The parameter specified by its name could not be found
ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0
The parameter specified by its offset could not be found
The parameter specified by its name could not be found
ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0
The parameter specified by its offset could not be found
Method has no parameters
