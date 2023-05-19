--TEST--
Lazy objects: makeLazy() on already lazy object is not allowed
--FILE--
<?php

class C extends stdClass {
    public int $a;
}

printf("# Ghost:\n");

$obj = new C();
$r = new ReflectionClass($obj);
$r->resetAsLazyGhost($obj, function () {});

try {
    $r->resetAsLazyGhost($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

printf("# Virtual:\n");

$obj = new C();
$r->resetAsLazyProxy($obj, function () {});

try {
    $r->resetAsLazyProxy($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$obj = new C();
$r->resetAsLazyProxy($obj, function () {
    return new C();
});
$r->initializeLazyObject($obj);

try {
    $r->resetAsLazyProxy($obj, function ($obj) {
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
==DONE==
--EXPECT--
# Ghost:
ReflectionException: Object is already lazy
# Virtual:
ReflectionException: Object is already lazy
==DONE==
