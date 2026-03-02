--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() may realize object if last lazy prop
--FILE--
<?php

class C {
    public $a;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 'test');
    $reflector->getProperty('b')->setRawValueWithoutLazyInitialization($obj, 'test');

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
  string(4) "test"
  ["b"]=>
  string(4) "test"
}
# Proxy
bool(true)
object(C)#%d (2) {
  ["a"]=>
  string(4) "test"
  ["b"]=>
  string(4) "test"
}
