--TEST--
Lazy objects: Object is still lazy after initializer exception (overridden prop)
--FILE--
<?php

class B {
    public int $b = 1;
}
class C extends B {
    public $a = 1;
    public int $b = 2;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj::class);
    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $i = 5;
    $obj->b = &$i;

    try {
        $reflector->initializeLazyObject($obj);
    } catch (Exception $e) {
        printf("%s\n", $e->getMessage());
    }

    printf("Is lazy: %d\n", (new ReflectionClass($obj))->isUninitializedLazyObject($obj));
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    throw new Exception('initializer exception');
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    throw new Exception('initializer exception');
});

test('Virtual', $obj);

--EXPECT--
# Ghost:
string(11) "initializer"
initializer exception
Is lazy: 1
# Virtual:
string(11) "initializer"
initializer exception
Is lazy: 1
