--TEST--
Lazy objects: ReflectionClass::isUninitializedLazyObject()
--FILE--
<?php

class C {
    public int $a;
}

function test(string $name, object $obj) {
    $reflector = new ReflectionClass(C::class);

    printf("# %s\n", $name);

    var_dump($reflector->isUninitializedLazyObject($obj));
    var_dump($obj->a);
    var_dump($reflector->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->a = 1;
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $obj = new C();
    $obj->a = 1;
    return $obj;
});

test('Proxy', $obj);

?>
--EXPECT--
# Ghost
bool(true)
string(11) "initializer"
int(1)
bool(false)
# Proxy
bool(true)
string(11) "initializer"
int(1)
bool(false)
