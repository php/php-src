--TEST--
Lazy objects: dynamic property op error
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a = 1;
    public function __construct() {
        var_dump(__METHOD__);
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    try {
        var_dump(@$obj->dynamic++);
    } catch(Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    throw new Error("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    throw new Error("initializer");
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
Error: initializer
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
# Proxy:
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
Error: initializer
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
