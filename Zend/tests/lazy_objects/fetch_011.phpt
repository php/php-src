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

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    throw new Error("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    throw new Error("initializer");
});

test('Virtual', $obj);

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
# Virtual:
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
Error: initializer
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
