--TEST--
Lazy objects: skipLazyInitialization() has no effect on initialized objects
--FILE--
<?php

class C {
    public function __construct() {
        $this->a = 2;
    }
    public $a = 1;
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->initializeLazyObject($obj);
    $reflector->getProperty('a')->skipLazyInitialization($obj);

    var_dump($obj->a);
    var_dump(!$reflector->isUninitializedLazyObject($obj));
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
bool(true)
object(C)#%d (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  NULL
}
# Proxy
int(1)
bool(true)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(2)
    ["b"]=>
    NULL
  }
}
