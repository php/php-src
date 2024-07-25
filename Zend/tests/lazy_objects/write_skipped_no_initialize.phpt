--TEST--
Lazy objects: write to skipped property does not initialize object
--FILE--
<?php

class C {
    public $a;
    public int $b = 1;
    public int $c;

    public function __construct() {
        var_dump(__METHOD__);
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $reflector->getProperty('c')->skipLazyInitialization($obj);

    var_dump($obj);
    $obj->a = 2;
    $obj->b = 2;
    $obj->c = 2;
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
object(C)#%d (2) {
  ["a"]=>
  NULL
  ["b"]=>
  int(1)
  ["c"]=>
  uninitialized(int)
}
object(C)#%d (3) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(2)
  ["c"]=>
  int(2)
}
# Proxy:
object(C)#%d (2) {
  ["a"]=>
  NULL
  ["b"]=>
  int(1)
  ["c"]=>
  uninitialized(int)
}
object(C)#%d (3) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(2)
  ["c"]=>
  int(2)
}
