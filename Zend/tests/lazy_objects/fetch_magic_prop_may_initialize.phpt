--TEST--
Lazy objects: magic property fetch initializes object if magic method observes object state
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
# Proxy:
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
