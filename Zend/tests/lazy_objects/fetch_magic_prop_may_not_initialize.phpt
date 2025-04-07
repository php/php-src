--TEST--
Lazy objects: magic property fetch does not not initialize object if magic method does not observe object state
--FILE--
<?php

class C {
    public int $a = 1;
    public function __construct() {
        var_dump(__METHOD__);
    }
    public function __get($name) {
        return $name;
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
string(5) "magic"
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
# Proxy:
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
string(5) "magic"
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
