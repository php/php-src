--TEST--
Lazy objects: markLazyObjectAsInitialized() initializes properties to their default value and skips initializer
--FILE--
<?php

class C {
    public int $a = 1;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(C::class);

    printf("Initialized:\n");
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    printf("markLazyObjectAsInitialized(true) returns \$obj:\n");
    var_dump($reflector?->markLazyObjectAsInitialized($obj) === $obj);

    printf("Initialized:\n");
    var_dump(!$reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
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
Initialized:
bool(false)
markLazyObjectAsInitialized(true) returns $obj:
bool(true)
Initialized:
bool(true)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Proxy:
Initialized:
bool(false)
markLazyObjectAsInitialized(true) returns $obj:
bool(true)
Initialized:
bool(true)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
