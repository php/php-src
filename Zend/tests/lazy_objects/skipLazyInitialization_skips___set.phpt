--TEST--
Lazy objects: skipLazyInitialization() does not call __set()
--FILE--
<?php

class C {
    public $a = 1;
    public $b;

    public function __set($name, $value) {
        throw new \Exception(__METHOD__);
    }
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
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
int(1)
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Proxy
int(1)
bool(false)
lazy proxy object(C)#%d (1) {
  ["a"]=>
  int(1)
}
