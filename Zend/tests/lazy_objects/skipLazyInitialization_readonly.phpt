--TEST--
Lazy objects: skipLazyInitialization() preserves readonly semantics
--FILE--
<?php

class C {
    public function __construct() {
        $this->a = 1;
    }
    public readonly int $a;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->skipLazyInitialization($obj);

    try {
        var_dump($obj->a);
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    var_dump(!$reflector->isUninitializedLazyObject($obj));
    var_dump($obj);

    $reflector->initializeLazyObject($obj);
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
Error: Typed property C::$a must not be accessed before initialization
bool(false)
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
}
# Proxy
Error: Typed property C::$a must not be accessed before initialization
bool(false)
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(1)
    ["b"]=>
    NULL
  }
}
