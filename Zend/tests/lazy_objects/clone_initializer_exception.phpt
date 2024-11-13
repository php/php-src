--TEST--
Lazy objects: clone: initializer exception
--FILE--
<?php

class C {
    public $a = 1;

    public function __construct() {
    }

    public function __destruct() {
        var_dump(__METHOD__);
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj::class);

    try {
        $clone = clone $obj;
    } catch (\Exception $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    var_dump($reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    throw new \Exception('initializer');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    throw new \Exception('initializer');
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
Exception: initializer
bool(true)
lazy ghost object(C)#%d (0) {
}
# Proxy:
Exception: initializer
bool(true)
lazy proxy object(C)#%d (0) {
}
