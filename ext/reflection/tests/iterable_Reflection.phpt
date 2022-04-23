--TEST--
iterable Type in Reflection
--FILE--
<?php

$function = function(): iterable {};

$reflectionFunc = new ReflectionFunction($function);
$returnType = $reflectionFunc->getReturnType();
var_dump($returnType::class);
foreach ($returnType->getTypes() as $type) {
    var_dump($type->getName());
}

class PropIterableTypeTest {
    public iterable $iterable;
}

$reflector = new ReflectionClass(PropIterableTypeTest::class);

[$property] = $reflector->getProperties();
$iterableType = $property->getType();
var_dump($iterableType::class);
foreach ($iterableType->getTypes() as $type) {
    var_dump($type->getName());
}

?>
--EXPECT--
string(19) "ReflectionUnionType"
string(11) "Traversable"
string(5) "array"
string(19) "ReflectionUnionType"
string(11) "Traversable"
string(5) "array"
