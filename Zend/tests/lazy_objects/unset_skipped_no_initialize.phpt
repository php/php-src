--TEST--
Lazy objects: unset of undefined skipped property initializes object
--FILE--
<?php

class C {
    public $a;
    public int $b = 1;
    public int $c;

    public function __construct(int $a) {
        var_dump(__METHOD__);
        $this->a = $a;
        $this->b = 2;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $reflector->getProperty('c')->skipLazyInitialization($obj);

    var_dump($obj);
    unset($obj->a);
    unset($obj->b);
    unset($obj->c);
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct(1);
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C(1);
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
object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  uninitialized(int)
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
object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  uninitialized(int)
}
