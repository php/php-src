--TEST--
Lazy objects: fetch ref on skipped property does not initialize object
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

    var_dump($obj);
    $ref = &$obj->a;
    $ref = &$obj->b;
    try {
        $ref = &$obj->c;
    } catch (Error $e) {
        printf("%s\n", $e->getMessage());
    }
    var_dump($ref);
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
  ["c"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
Cannot access uninitialized non-nullable property C::$c by reference
int(1)
object(C)#%d (2) {
  ["a"]=>
  NULL
  ["b"]=>
  &int(1)
  ["c"]=>
  uninitialized(int)
}
# Proxy:
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
Cannot access uninitialized non-nullable property C::$c by reference
int(1)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    NULL
    ["b"]=>
    &int(1)
    ["c"]=>
    uninitialized(int)
  }
}
