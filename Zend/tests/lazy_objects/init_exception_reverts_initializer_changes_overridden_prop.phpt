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
    $reflector = new ReflectionClass(C::class);

    printf("# %s:\n", $name);

    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $i = 5;
    $obj->b = &$i;

    try {
        $reflector->initializeLazyObject($obj);
    } catch (Exception $e) {
        printf("%s\n", $e->getMessage());
    }

    printf("Is lazy: %d\n", $reflector->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    throw new Exception('initializer exception');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    throw new Exception('initializer exception');
});

test('Proxy', $obj);

--EXPECT--
# Ghost:
string(11) "initializer"
initializer exception
Is lazy: 1
# Proxy:
string(11) "initializer"
initializer exception
Is lazy: 1
