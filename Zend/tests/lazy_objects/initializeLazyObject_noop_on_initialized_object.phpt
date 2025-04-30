--TEST--
Lazy objects: ReflectionClass::initializeLazyObject() on an initialized object is a no-op
--FILE--
<?php

class C {
    public int $a;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(C::class);

    var_dump($obj->a);

    var_dump(!$reflector->isUninitializedLazyObject($obj));

    var_dump($reflector?->initializeLazyObject($obj));

    var_dump(!$reflector->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->a = 1;
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $c = new C();
    $c->a = 1;
    return $c;
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
string(11) "initializer"
int(1)
bool(true)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
bool(true)
# Proxy:
string(11) "initializer"
int(1)
bool(true)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
bool(true)
