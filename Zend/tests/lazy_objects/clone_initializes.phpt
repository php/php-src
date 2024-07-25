--TEST--
Lazy objects: clone initializes object
--FILE--
<?php

class C {
    public $a = 1;

    public function __construct() {
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj::class);
    $clone = clone $obj;

    var_dump($reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
    var_dump($reflector->isUninitializedLazyObject($clone));
    var_dump($clone);
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
string(11) "initializer"
bool(false)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
bool(false)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Proxy:
string(11) "initializer"
bool(false)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(1)
  }
}
bool(false)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(1)
  }
}
