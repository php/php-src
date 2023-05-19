--TEST--
Lazy objects: unset of magic property may initialize object
--FILE--
<?php

class C {
    public int $b = 1;

    public function __construct(int $a) {
        var_dump(__METHOD__);
        $this->b = 2;
    }

    public function __unset($name) {
        var_dump($this->b);
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
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
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
int(2)
object(C)#%d (1) {
  ["b"]=>
  int(2)
}
# Virtual:
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
int(2)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["b"]=>
    int(2)
  }
}
