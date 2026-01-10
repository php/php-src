--TEST--
Lazy objects: resetAsLazy*() preserves readonly semantics
--FILE--
<?php

class B {
    public readonly int $a;
    public readonly int $b;
    public int $c;

    public function __construct($value, bool $setB = false) {
        try {
            $this->a = $value;
        } catch (\Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
        if ($setB) {
            $this->b = $value;
        }
        try {
            $this->c = $value;
        } catch (\Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
}

final class C extends B {
}


function test(string $name, object $obj) {
    printf("# %s\n", $name);

    $reflector = new ReflectionClass($obj::class);

    $reflector->resetAsLazyGhost($obj, function ($obj) {
        $obj->__construct(2, setB: true);
    });

    $reflector->initializeLazyObject($obj);

    var_dump($obj);
}

$obj = new B(1);
test('B', $obj);

$obj = new C(1);
test('C', $obj);

--EXPECTF--
# B
object(B)#%d (3) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(2)
  ["c"]=>
  int(2)
}
# C
Error: Cannot modify readonly property B::$a
object(C)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(2)
}
