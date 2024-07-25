--TEST--
Lazy objects: serialize() does not initialize object if __serialize does observe object state
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

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
string(12) "O:1:"C":0:{}"
object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
# Proxy:
string(12) "O:1:"C":0:{}"
object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
