--TEST--
Lazy objects: setRawValueWithoutLazyInitialization() may trigger initialization via side effects (__toString())
--FILE--
<?php

class C {
    public function __construct() {
        printf("%s\n", __METHOD__);
        $this->a = 'a';
        $this->b = 'b';
    }
    public string $a;
    public string $b;
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);

    $value = new class($obj) {
        function __construct(public object $obj) {}
        function __toString() {
            return $this->obj->b;
        }
    };
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, $value);

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
C::__construct
bool(true)
object(C)#%d (2) {
  ["a"]=>
  string(1) "b"
  ["b"]=>
  string(1) "b"
}
# Proxy
C::__construct
bool(true)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
}
