--TEST--
Lazy objects: Initializer effects are reverted after exception (properties hashtable)
--FILE--
<?php

class C {
    public $a = 1;
    public int $b = 2;
    public int $c;
}

function test(string $name, object $obj) {
    $reflector = new ReflectionClass(C::class);

    printf("# %s:\n", $name);

    (new ReflectionProperty(C::class, 'c'))->setRawValueWithoutLazyInitialization($obj, 0);

    // Builds properties hashtable
    var_dump(get_mangled_object_vars($obj));

    try {
        $reflector->initializeLazyObject($obj);
    } catch (Exception $e) {
        printf("%s\n", $e->getMessage());
    }

    var_dump($obj);
    printf("Is lazy: %d\n", $reflector->isUninitializedLazyObject($obj));
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    $obj->c = 5;
    throw new Exception('initializer exception');
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $obj->a = 3;
    $obj->b = 4;
    $obj->c = 5;
    throw new Exception('initializer exception');
});

// Initializer effects on the proxy are not reverted
test('Proxy', $obj);

--EXPECTF--
# Ghost:
array(1) {
  ["c"]=>
  int(0)
}
string(11) "initializer"
initializer exception
lazy ghost object(C)#%d (1) {
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  int(0)
}
Is lazy: 1
# Proxy:
array(1) {
  ["c"]=>
  int(0)
}
string(11) "initializer"
initializer exception
lazy proxy object(C)#%d (3) {
  ["a"]=>
  int(3)
  ["b"]=>
  int(4)
  ["c"]=>
  int(5)
}
Is lazy: 1
