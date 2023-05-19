--TEST--
Lazy objects: Object is not lazy anymore if all props have been skipped
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
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass($obj);
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    $reflector->getProperty('a')->skipLazyInitialization($obj);
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    // Should not count a second prop initialization
    $reflector->getProperty('a')->skipLazyInitialization($obj);
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    try {
        // Should not count a prop initialization
        $reflector->getProperty('xxx')->skipLazyInitialization($obj);
    } catch (ReflectionException $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    try {
        // Should not count a prop initialization
        $reflector->getProperty('b')->skipLazyInitialization($obj);
    } catch (ReflectionException $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }

    (new ReflectionProperty(B::class, 'b'))->skipLazyInitialization($obj);
    var_dump(!$reflector->isUninitializedLazyObject($obj));

    var_dump($obj);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Virtual', $obj);

--EXPECTF--
# Ghost:
bool(false)
bool(false)
bool(false)
ReflectionException: Property C::$xxx does not exist
ReflectionException: Property C::$b does not exist
bool(true)
object(C)#%d (0) {
  ["b":"B":private]=>
  uninitialized(string)
  ["a"]=>
  uninitialized(string)
}
# Virtual:
bool(false)
bool(false)
bool(false)
ReflectionException: Property C::$xxx does not exist
ReflectionException: Property C::$b does not exist
bool(true)
object(C)#%d (0) {
  ["b":"B":private]=>
  uninitialized(string)
  ["a"]=>
  uninitialized(string)
}
