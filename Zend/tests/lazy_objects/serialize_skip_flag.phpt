--TEST--
Lazy objects: serialize does not initializes object if flag is set
--FILE--
<?php

class C {
    public int $a;
    public int $b;
}


function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    (new ReflectionProperty($obj, 'b'))->setRawValueWithoutLazyInitialization($obj, 1);

    $serialized = serialize($obj);
    $unserialized = unserialize($serialized);
    var_dump($serialized, $unserialized);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Proxy', $obj);

--EXPECTF--
# Ghost:
string(24) "O:1:"C":1:{s:1:"b";i:1;}"
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  int(1)
}
# Proxy:
string(24) "O:1:"C":1:{s:1:"b";i:1;}"
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  int(1)
}
