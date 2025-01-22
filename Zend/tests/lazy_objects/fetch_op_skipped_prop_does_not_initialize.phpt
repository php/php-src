--TEST--
Lazy objects: property op on skipped property does not initialize object
--FILE--
<?php

class C {
    public $a;
    public int $b = 1;
    public int $c;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 2;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj);
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $reflector->getProperty('c')->skipLazyInitialization($obj);

    var_dump($obj);
    var_dump($obj->a++);
    var_dump($obj->b++);
    try {
        var_dump($obj->c++);
    } catch (Error $e) {
        printf("%s\n", $e->getMessage());
    }
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
    return new c();
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
NULL
int(1)
Typed property C::$c must not be accessed before initialization
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
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
NULL
int(1)
Typed property C::$c must not be accessed before initialization
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  uninitialized(int)
}
