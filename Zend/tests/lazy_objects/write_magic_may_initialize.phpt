--TEST--
Lazy objects: property write to magic property initializes object if method updates object state
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
    $obj->magic = 3;
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
}
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (1) {
  ["a"]=>
  int(3)
}
# Proxy:
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
