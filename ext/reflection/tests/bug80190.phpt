--TEST--
Bug #80190: ReflectionMethod::getReturnType() does not handle static as part of union type
--FILE--
<?php

class C
{
    public function a(): self
    {
    }

    public function b(): stdClass|self
    {
    }

    public function c(): static
    {
    }

    public function d(): stdClass|static
    {
    }
}

foreach ((new ReflectionClass(C::class))->getMethods() as $method) {
    print $method->getDeclaringClass()->getName() . '::' . $method->getName() . '()' . PHP_EOL;
    print '    $method->getReturnType() returns ' . get_class($method->getReturnType()) . PHP_EOL;
    print '    $method->getReturnType()->__toString() returns ' . $method->getReturnType() . PHP_EOL;

    if ($method->getReturnType() instanceof ReflectionUnionType) {
        print '    $method->getReturnType()->getTypes() returns an array with ' . count($method->getReturnType()->getTypes()) . ' element(s)' . PHP_EOL;

        print '    type(s) in union: ';
        
        $types = [];

        foreach ($method->getReturnType()->getTypes() as $type) {
            $types[] = get_class($type) . "($type)";
        }
        
        print join(', ', $types) . PHP_EOL;
    }

    print PHP_EOL;
}

?>
--EXPECT--
C::a()
    $method->getReturnType() returns ReflectionNamedType
    $method->getReturnType()->__toString() returns self

C::b()
    $method->getReturnType() returns ReflectionUnionType
    $method->getReturnType()->__toString() returns stdClass|self
    $method->getReturnType()->getTypes() returns an array with 2 element(s)
    type(s) in union: ReflectionNamedType(stdClass), ReflectionNamedType(self)

C::c()
    $method->getReturnType() returns ReflectionNamedType
    $method->getReturnType()->__toString() returns static

C::d()
    $method->getReturnType() returns ReflectionUnionType
    $method->getReturnType()->__toString() returns stdClass|static
    $method->getReturnType()->getTypes() returns an array with 2 element(s)
    type(s) in union: ReflectionNamedType(stdClass), ReflectionNamedType(static)
