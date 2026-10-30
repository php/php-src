--TEST--
Lazy objects: serialize does not initializes object with __sleep method if flag is set
--FILE--
<?php

class C {
    public int $a;
    public function __sleep() {
        return ['a'];
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
    $obj->a = 1;
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $obj->a = 1;
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

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
