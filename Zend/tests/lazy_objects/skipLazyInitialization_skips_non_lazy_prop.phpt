--TEST--
Lazy objects: skipLazyInitialization() has no effect on non-lazy properties
--FILE--
<?php

class C {
    public $a = 1;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->skipLazyInitialization($obj);

    $obj->a = 2;

    $reflector->getProperty('a')->skipLazyInitialization($obj);

    var_dump($obj->a);
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
int(2)
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  int(2)
}
# Proxy
int(2)
bool(false)
lazy proxy object(C)#%d (1) {
  ["a"]=>
  int(2)
}
