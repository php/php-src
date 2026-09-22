--TEST--
Lazy objects: JIT: ASSIGN_OBJ_OP with dynamic prop
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $b = 1;

    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
        $this->b = 3;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    $obj->a += 1;
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (2) {
  ["b"]=>
  int(1)
  ["a"]=>
  int(2)
}
# Proxy:
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["b"]=>
    int(3)
    ["a"]=>
    int(2)
  }
}
