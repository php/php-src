--TEST--
Lazy objects: fetch skipped property does not initialize object
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

    $reflector = new ReflectionClass($obj);
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    $reflector->getProperty('b')->skipLazyInitialization($obj);
    $reflector->getProperty('c')->skipLazyInitialization($obj);

    var_dump($obj);
    var_dump($obj->a);
    var_dump($obj->b);
    try {
        var_dump($obj->c);
    } catch (Error $e) {
        printf("%s\n", $e->getMessage());
    }
    var_dump($obj);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct(1);
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C(1);
});

test('Virtual', $obj);

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
  NULL
  ["b"]=>
  int(1)
  ["c"]=>
  uninitialized(int)
}
# Virtual:
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
  NULL
  ["b"]=>
  int(1)
  ["c"]=>
  uninitialized(int)
}
