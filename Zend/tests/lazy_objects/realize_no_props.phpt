--TEST--
Lazy objects: Object with no props is never lazy
--FILE--
<?php

class C {}

#[AllowDynamicProperties]
class D {}

function test(string $name, object $obj, object $obj2, object $obj3) {
    printf("# %s:\n", $name);

    var_dump((new ReflectionClass($obj::class))->isUninitializedLazyObject($obj));
    var_dump($obj);

    var_dump((new ReflectionClass($obj2::class))->isUninitializedLazyObject($obj2));
    var_dump($obj2);

    var_dump((new ReflectionClass($obj3::class))->isUninitializedLazyObject($obj3));
    var_dump($obj3);
}

$obj = new C();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
});

$obj2 = new D();
$obj2->dynamic = 'value';
(new ReflectionClass($obj2))->resetAsLazyGhost($obj2, function ($obj2) {
    var_dump("initializer");
});

$obj3 = (new ReflectionClass(C::class))->newLazyGhost(function () {
    var_dump("initializer");
});

test('Ghost', $obj, $obj2, $obj3);

$obj = new C();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
});

$obj2 = new D();
$obj2->dynamic = 'value';
(new ReflectionClass($obj2))->resetAsLazyProxy($obj2, function ($obj2) {
    var_dump("initializer");
});

$obj3 = (new ReflectionClass(C::class))->newLazyGhost(function () {
    var_dump("initializer");
});

test('Proxy', $obj, $obj2, $obj3);

--EXPECTF--
# Ghost:
bool(false)
object(C)#%d (0) {
}
bool(false)
object(D)#%d (0) {
}
bool(false)
object(C)#%d (0) {
}
# Proxy:
bool(false)
object(C)#%d (0) {
}
bool(false)
object(D)#%d (0) {
}
bool(false)
object(C)#%d (0) {
}
