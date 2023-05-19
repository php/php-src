--TEST--
Lazy objects: __sleep may initialize object
--FILE--
<?php

class C {
    public int $a;
    public function __sleep() {
        var_dump($this->a);
        return ['a'];
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
    $obj->a = 1;
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    $c = new C();
    $c->a = 1;
    return $c;
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Virtual', $obj);

--EXPECTF--
# Ghost:
string(11) "initializer"
int(1)
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Virtual:
string(11) "initializer"
int(1)
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
