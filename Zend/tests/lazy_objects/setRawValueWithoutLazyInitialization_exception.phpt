--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() leaves property as lazy if exception prevents update
--FILE--
<?php

class C {
    public function __construct() {
        printf("%s\n", __METHOD__);
    }
    public int $a;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    try {
        $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, new stdClass);
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    // Prop is still lazy: This triggers initialization
    $obj->a = 1;
    var_dump(!$reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function () {
    return new C();
});

test('Proxy', $obj);

?>
--EXPECTF--
# Ghost
TypeError: Cannot assign stdClass to property C::$a of type int
C::__construct
bool(true)
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
}
# Proxy
TypeError: Cannot assign stdClass to property C::$a of type int
C::__construct
bool(true)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(1)
    ["b"]=>
    NULL
  }
}
