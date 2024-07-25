--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() preserves readonly semantics
--FILE--
<?php

class C {
    public function __construct() {
        try {
            $this->a = 1;
        } catch (Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
    public readonly int $a;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 2);

    var_dump($obj->a);
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
int(2)
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  int(2)
}
Error: Cannot modify readonly property C::$a
object(C)#%d (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  NULL
}
# Proxy
int(2)
bool(false)
lazy proxy object(C)#%d (1) {
  ["a"]=>
  int(2)
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
