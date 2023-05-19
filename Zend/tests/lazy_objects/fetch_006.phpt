--TEST--
Lazy objects: magic property fetch may initialize object
--FILE--
<?php

class C {
    public int $a = 1;
    public function __construct() {
        var_dump(__METHOD__);
    }
    public function __get($name) {
        return $this->a;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    var_dump($obj->magic);
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
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
int(1)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Virtual:
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
int(1)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(1)
  }
}
