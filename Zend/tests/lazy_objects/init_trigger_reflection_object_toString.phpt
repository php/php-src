--TEST--
Lazy objects: ReflectionObject::__toString() does not trigger initialization
--FILE--
<?php

class C {
    public int $a;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    (new ReflectionObject($obj))->__toString();

    printf("Initialized:\n");
    var_dump(!(new ReflectionClass($obj))->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Proxy', $obj);

--EXPECT--
# Ghost
Initialized:
bool(false)
# Proxy
Initialized:
bool(false)
