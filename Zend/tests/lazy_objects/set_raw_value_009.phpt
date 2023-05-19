--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() preserves readonly semantics
--FILE--
<?php

class C {
    public readonly int $a;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 1);
    try {
        $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 2);
    } catch (\Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    var_dump(!$reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function () {
    throw new \Exception('initializer');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function () {
    throw new \Exception('initializer');
});

test('Proxy', $obj);

?>
--EXPECTF--
# Ghost
Error: Cannot modify readonly property C::$a
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Proxy
Error: Cannot modify readonly property C::$a
bool(false)
lazy proxy object(C)#%d (1) {
  ["a"]=>
  int(1)
}
