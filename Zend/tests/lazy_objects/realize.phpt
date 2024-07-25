--TEST--
Lazy objects: Object is not lazy anymore if all props have been assigned a value
--FILE--
<?php

#[AllowDynamicProperties]
class B {
    private readonly string $b;

    public function __construct() {
        $this->b = 'b';
    }
}

#[AllowDynamicProperties]
class C extends B {
    public string $a;

    public function __construct() {
        parent::__construct();
        $this->a = 'a';
    }
}

function test(string $name, object $obj) {
    $reflector = new ReflectionClass(C::class);

    printf("# %s:\n", $name);

    var_dump(!$reflector->isUninitializedLazyObject($obj));

    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 'a1');
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    // Should not count a second prop initialization
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 'a2');
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    try {
        // Should not count a prop initialization
        $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, new stdClass);
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    // Should not count a prop initialization
    //$reflector->getProperty('b')->setRawValueWithoutLazyInitialization($obj, 'dynamic B');
    //var_dump(!$reflector->isUninitializedLazyObject($obj));

    (new ReflectionProperty(B::class, 'b'))->setRawValueWithoutLazyInitialization($obj, 'b');
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Proxy', $obj);

--EXPECTF--
# Ghost:
bool(false)
bool(false)
bool(false)
TypeError: Cannot assign stdClass to property C::$a of type string
bool(true)
object(C)#%d (2) {
  ["b":"B":private]=>
  string(1) "b"
  ["a"]=>
  string(2) "a2"
}
# Proxy:
bool(false)
bool(false)
bool(false)
TypeError: Cannot assign stdClass to property C::$a of type string
bool(true)
object(C)#%d (2) {
  ["b":"B":private]=>
  string(1) "b"
  ["a"]=>
  string(2) "a2"
}
