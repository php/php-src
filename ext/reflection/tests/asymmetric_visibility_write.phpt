--TEST--
ReflectionProperty::is{Private,Protected}Set
--FILE--
<?php

class Foo {
    public private(set) int $bar = 0;
    public protected(set) int $baz = 0;
}

function test($property) {
    static $i = 0;
    $foo = new Foo();
    $reflectionProperty = new ReflectionProperty(Foo::class, $property);
    $reflectionProperty->setValue($foo, $i++);
    var_dump($reflectionProperty->getValue($foo));
}

test('bar');
test('baz');

?>
--EXPECT--
int(0)
int(1)
