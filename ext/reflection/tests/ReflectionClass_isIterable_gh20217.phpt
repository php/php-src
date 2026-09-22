--TEST--
GH-20217 (ReflectionClass::isIterable() should return false for classes with property hooks)
--FILE--
<?php

class ClassWithPropertyHooks
{
    public string $name {
        get => 'virtual';
    }
}

class IterableClassWithPropertyHooks implements IteratorAggregate
{
    public string $name {
        get => 'virtual';
    }

    public function getIterator(): Traversable
    {
        return new ArrayIterator([]);
    }
}

$classes = [
    'ClassWithPropertyHooks' => false,
    'IterableClassWithPropertyHooks' => true,
];

foreach ($classes as $className => $expected) {
    $status = (new ReflectionClass($className)->isIterable() === $expected) ? 'PASS' : 'FAIL';
    echo "$className: $status\n";
}

?>
--EXPECT--
ClassWithPropertyHooks: PASS
IterableClassWithPropertyHooks: PASS
