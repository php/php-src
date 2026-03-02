--TEST--
Lazy objects: resetAsLazy*() on already lazy object is not allowed
--FILE--
<?php

class C extends stdClass {
    public int $a;
}

$reflector = new ReflectionClass(C::class);

printf("# Ghost:\n");

$obj = new C();
$reflector->resetAsLazyGhost($obj, function () {});

try {
    $reflector->resetAsLazyGhost($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

printf("# Proxy:\n");

$obj = new C();
$reflector->resetAsLazyProxy($obj, function () {});

try {
    $reflector->resetAsLazyProxy($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$obj = new C();
$reflector->resetAsLazyProxy($obj, function () {
    return new C();
});
$reflector->initializeLazyObject($obj);

try {
    $reflector->resetAsLazyProxy($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
==DONE==
--EXPECT--
# Ghost:
ReflectionException: Object is already lazy
# Proxy:
ReflectionException: Object is already lazy
==DONE==
