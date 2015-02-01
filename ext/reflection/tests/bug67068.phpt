--TEST--
Bug #67068 (ReflectionFunction::getClosure returns something that doesn't report as a closure)
--FILE--
<?php
class MyClass {
    public function method() {}
}

$object = new MyClass;
$reflector = new \ReflectionMethod($object, 'method');
$closure = $reflector->getClosure($object);

$closureReflector = new \ReflectionFunction($closure);

var_dump($closureReflector->isClosure());
?>
--EXPECT--
bool(true)