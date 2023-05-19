--TEST--
Lazy objects: circular property write to magic property initializes object
--FILE--
<?php

class C {
    public $a;

    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }

    public function __set($name, $value) {
        $this->a = $value;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    $obj->a = 3;
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
}
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (1) {
  ["a"]=>
  int(3)
}
# Virtual:
lazy proxy object(C)#%d (0) {
}
string(11) "initializer"
string(14) "C::__construct"
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(3)
  }
}
