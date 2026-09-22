--TEST--
Lazy objects: ReflectionClass::initializeLazyObject() error
--FILE--
<?php

class C {
    public int $a;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(C::class);
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    try {
        var_dump($reflector?->initializeLazyObject($obj));
    } catch (Exception $e) {
        printf("%s\n", $e->getMessage());
    }

    var_dump(!$reflector->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    throw new \Exception('initializer exception');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    throw new \Exception('initializer exception');
});

test('Proxy', $obj);

--EXPECT--
# Ghost:
bool(false)
string(11) "initializer"
initializer exception
bool(false)
# Proxy:
bool(false)
string(11) "initializer"
initializer exception
bool(false)
