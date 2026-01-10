--TEST--
Lazy objects: resetAsLazy*() on object being initialized
--FILE--
<?php

class C {
    public int $a;
}

$reflector = new ReflectionClass(C::class);

printf("# Ghost:\n");

$obj = new C();
$reflector->resetAsLazyGhost($obj, function ($obj) use ($reflector) {
    try {
        $reflector->resetAsLazyGhost($obj, function () { });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    try {
        $reflector->resetAsLazyProxy($obj, function () { });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

});
$reflector->initializeLazyObject($obj);

printf("# Proxy:\n");

$obj = new C();
$reflector->resetAsLazyProxy($obj, function ($obj) use ($reflector) {
    try {
        $reflector->resetAsLazyProxy($obj, function () { });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    try {
        $reflector->resetAsLazyGhost($obj, function () { });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    return new C();
});
$reflector->initializeLazyObject($obj);

?>
==DONE==
--EXPECT--
# Ghost:
Error: Can not reset an object while it is being initialized
Error: Can not reset an object while it is being initialized
# Proxy:
Error: Can not reset an object while it is being initialized
Error: Can not reset an object while it is being initialized
==DONE==
