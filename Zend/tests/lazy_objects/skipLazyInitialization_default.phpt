--TEST--
Lazy objects: skipLazyInitialization() marks property as non-lazy and sets default value if any
--FILE--
<?php

class C {
    public $a;
    public $b = 1;
    public $c;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    $reflector->getProperty('b')->skipLazyInitialization($obj);

    var_dump($obj->a);
    var_dump($obj->b);
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
NULL
int(1)
bool(false)
lazy ghost object(C)#%d (2) {
  ["a"]=>
  NULL
  ["b"]=>
  int(1)
}
# Proxy
NULL
int(1)
bool(false)
lazy proxy object(C)#%d (2) {
  ["a"]=>
  NULL
  ["b"]=>
  int(1)
}
