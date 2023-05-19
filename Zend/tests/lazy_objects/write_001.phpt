--TEST--
Lazy objects: property write initializes object
--FILE--
<?php

class C {
    public $a;
    public int $b = 1;

    public function __construct() {
        var_dump(__METHOD__);
        $this->b = 3;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    $obj->a = 2;
    var_dump($obj);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
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
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(3)
}
# Virtual:
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(2)
    ["b"]=>
    int(3)
  }
}
