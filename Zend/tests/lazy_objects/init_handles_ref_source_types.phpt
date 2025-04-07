--TEST--
Lazy objects: Pre-initialization reference source types are properly handled (no initialization exception)
--FILE--
<?php

class C {
    public ?C $a;
    public ?C $b;
    public $c;
    public function __construct() {
        $this->a = null;
        unset($this->b);
        $this->b = null;
    }
}

function test(string $name, object $obj) {
    $reflector = new ReflectionClass(C::class);

    printf("# %s:\n", $name);

    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, null);
    $refA = &$obj->a;
    $reflector->getProperty('b')->setRawValueWithoutLazyInitialization($obj, null);
    $refB = &$obj->b;

    var_dump($obj);
    var_dump($obj->c);
    var_dump($obj);

    try {
        // $refA retained its reference source type (except for the proxy
        // case: its the responsibility of the initializer to propagate
        // pre-initialized properties to the instance)
        $refA = 1;
    } catch (\Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    // source type was not duplicated
    unset($obj->a);
    $refA = 1;

    $refB = 1;
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C(null);
});

test('Proxy', $obj);
--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (2) {
  ["a"]=>
  &NULL
  ["b"]=>
  &NULL
}
string(11) "initializer"
NULL
object(C)#%d (3) {
  ["a"]=>
  &NULL
  ["b"]=>
  NULL
  ["c"]=>
  NULL
}
TypeError: Cannot assign int to reference held by property C::$a of type ?C
# Proxy:
lazy proxy object(C)#%d (2) {
  ["a"]=>
  &NULL
  ["b"]=>
  &NULL
}
string(11) "initializer"
NULL
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (3) {
    ["a"]=>
    NULL
    ["b"]=>
    NULL
    ["c"]=>
    NULL
  }
}
