--TEST--
iterable Type in Reflection
--FILE--
<?php

$functions = [
    function(): iterable {},
];

foreach ($functions as $function) {
    $reflectionFunc = new ReflectionFunction($function);
    $returnType = $reflectionFunc->getReturnType();
    var_dump($returnType->getName());
}


class PropIterableTypeTest {
    public iterable $iterable;
}

$reflector = new ReflectionClass(PropIterableTypeTest::class);

foreach ($reflector->getProperties() as $name => $property) {
    if ($property->hasType()) {
        printf("public %s $%s;\n",
            $property->getType()->getName(), $property->getName());
    } else printf("public $%s;\n", $property->getName());
}

?>
--EXPECT--
string(17) "Traversable|array"
public Traversable|array $iterable;
