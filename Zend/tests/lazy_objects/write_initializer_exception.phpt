--TEST--
Lazy objects: property write with initializer exception
--FILE--
<?php

class C {
    public $a;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    try {
        $obj->a = 1;
    } catch (Exception $e) {
        printf("%s\n", $e->getMessage());
    }
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    throw new \Exception('init exception');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    throw new \Exception('init exception');
});

test('Ghost', $obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) {
}
init exception
lazy ghost object(C)#%d (0) {
}
# Ghost:
lazy proxy object(C)#%d (0) {
}
init exception
lazy proxy object(C)#%d (0) {
}
