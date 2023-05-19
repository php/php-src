--TEST--
Lazy objects: serialize does not force initialization of object if the __serialize method is used
--FILE--
<?php

class C {
    public int $a;
    public function __serialize() {
        return [];
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $serialized = serialize($obj);
    $unserialized = unserialize($serialized);
    var_dump($serialized, $unserialized);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Virtual', $obj);

--EXPECTF--
# Ghost:
string(12) "O:1:"C":0:{}"
object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
# Virtual:
string(12) "O:1:"C":0:{}"
object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
