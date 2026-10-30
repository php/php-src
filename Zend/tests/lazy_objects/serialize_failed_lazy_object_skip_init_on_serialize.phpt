--TEST--
Lazy objects: serialize() a lazy object that previously failed to initialize, with SKIP_INITIALIZATION_ON_SERIALIZE
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a;
    public $b;
}

$reflector = new ReflectionClass(C::class);

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    try {
        $reflector->initializeLazyObject($obj);
    } catch (Exception $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    try {
        var_dump(unserialize(serialize($obj)));
    } catch (Exception $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}

$obj = $reflector->newLazyGhost(function ($obj) {
    throw new \Exception('Initializer');
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function () {
    throw new \Exception('Initializer');
}, ReflectionClass::SKIP_INITIALIZATION_ON_SERIALIZE);

test('Proxy', $obj);

?>
--EXPECTF--
# Ghost
Exception: Initializer
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  NULL
}
# Proxy
Exception: Initializer
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  NULL
}
