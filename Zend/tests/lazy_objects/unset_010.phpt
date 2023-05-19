--TEST--
Lazy objects: cannot unset hooked property
--FILE--
<?php

class C {
    public $a {
        get { return $this->a; }
        set($value) { $this->a = $value; }
    }
    public int $b = 1;

    public function __construct(int $a) {
        var_dump(__METHOD__);
        $this->a = $a;
        $this->b = 2;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj);
    try {
        unset($obj->a);
    } catch (\Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    var_dump($obj);
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct(1);
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C(1);
});

test('Virtual', $obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
Error: Cannot unset hooked property C::$a
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
# Virtual:
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
Error: Cannot unset hooked property C::$a
lazy proxy object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
