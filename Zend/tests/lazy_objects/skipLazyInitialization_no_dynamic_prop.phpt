--TEST--
Lazy objects: skipLazyInitialization() can not be used on dynamic properties
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $a = 1;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $c = new C();
    $c->dyn = 1;
    $propReflector = new ReflectionProperty($c, 'dyn');

    try {
        $propReflector->skipLazyInitialization($obj);
    } catch (\ReflectionException $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
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
--EXPECT--
# Ghost
ReflectionException: Can not use skipLazyInitialization on dynamic property C::$dyn
# Proxy
ReflectionException: Can not use skipLazyInitialization on dynamic property C::$dyn
