--TEST--
Lazy objects: Conversion to array
--FILE--
<?php

class C {
    public int $a;
    public $b;

    public function __construct() {
        $this->a = 1;
        $this->b = 2;
    }
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 3);

    $a = [];
    // Converts $obj to array internally
    array_splice($a, 0, 0, $obj);
    var_dump($a, $obj);

    $reflector->initializeLazyObject($obj);

    $a = [];
    array_splice($a, 0, 0, $obj);
    var_dump($a, $obj);
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
array(1) {
  [0]=>
  int(3)
}
lazy ghost object(C)#%d (1) {
  ["a"]=>
  int(3)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
# Proxy
array(1) {
  [0]=>
  int(3)
}
lazy proxy object(C)#%d (1) {
  ["a"]=>
  int(3)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
  }
}
