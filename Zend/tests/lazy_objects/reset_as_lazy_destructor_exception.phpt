--TEST--
Lazy objects: Destructor exception in resetAsLazy*()
--FILE--
<?php

class C {
    public readonly int $a;

    public function __construct() {
        $this->a = 1;
    }

    public function __destruct() {
        throw new \Exception(__METHOD__);
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = new C();
try {
    $reflector->resetAsLazyGhost($obj, function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

// Object was not made lazy
var_dump(!$reflector->isUninitializedLazyObject($obj));

print "# Proxy:\n";

$obj = new C();
try {
    (new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
        var_dump("initializer");
        return new C();
    });
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

// Object was not made lazy
var_dump(!(new ReflectionClass($obj))->isUninitializedLazyObject($obj));

?>
--EXPECT--
# Ghost:
Exception: C::__destruct
bool(true)
# Proxy:
Exception: C::__destruct
bool(true)
