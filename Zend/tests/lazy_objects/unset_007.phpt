--TEST--
Lazy objects: unset of undefined dynamic property initializes object (2)
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $b = 1;

    public function __construct(int $a) {
        var_dump(__METHOD__);
        $this->b = 2;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    // no var_dump($obj), so that properties ht is not initialized
    var_dump('before unset');
    unset($obj->a);
    var_dump($obj);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct(1);
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C(1);
});

test('Virtual', $obj);

--EXPECTF--
# Ghost:
string(12) "before unset"
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (1) {
  ["b"]=>
  int(2)
}
# Virtual:
string(12) "before unset"
string(11) "initializer"
string(14) "C::__construct"
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["b"]=>
    int(2)
  }
}
