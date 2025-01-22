--TEST--
Lazy objects: skipLazyInitialization() may realize object if last lazy prop
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
    $reflector->getProperty('b')->skipLazyInitialization($obj);

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
bool(true)
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
}
# Proxy
bool(true)
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
}
