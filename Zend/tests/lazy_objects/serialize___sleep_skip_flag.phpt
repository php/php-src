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
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
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
