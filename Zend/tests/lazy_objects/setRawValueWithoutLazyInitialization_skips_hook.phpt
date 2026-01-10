--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() skips hooks
--FILE--
<?php

class C {
    public $a {
        set($value) { throw new \Exception('hook'); $this->a = $value; }
        get { return $this->a; }
    }
    public $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 'test');

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
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  string(4) "test"
}
# Proxy
bool(false)
lazy proxy object(C)#%d (1) {
  ["a"]=>
  string(4) "test"
}
