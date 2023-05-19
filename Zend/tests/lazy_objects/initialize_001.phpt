--TEST--
Lazy objects: ReflectionClass::initializeLazyObject()
--FILE--
<?php

class C {
    public int $a;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj);

    printf("Initialized:\n");
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    var_dump($reflector?->initializeLazyObject($obj));

    printf("Initialized:\n");
    var_dump(!$reflector->isUninitializedLazyObject($obj));
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->a = 1;
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    $c = new C();
    $c->a = 1;
    return $c;
});

test('Virtual', $obj);

--EXPECTF--
# Ghost:
Initialized:
bool(false)
string(11) "initializer"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
Initialized:
bool(true)
# Virtual:
Initialized:
bool(false)
string(11) "initializer"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
Initialized:
bool(true)
