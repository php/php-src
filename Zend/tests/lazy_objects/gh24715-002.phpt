--TEST--
GH-24715 002: Object reset as lazy by a destructor triggered when releasing the initializer in markLazyObjectAsInitialized()
--FILE--
<?php

class C {
    public $a;
}

class D {
    public function __destruct() {
        global $obj, $reflector;
        try {
            $reflector->resetAsLazyGhost($obj, function () {});
        } catch (Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
}

$reflector = new ReflectionClass(C::class);
$d = new D();
$obj = $reflector->newLazyGhost(function ($obj) use ($d) {});
unset($d);

var_dump($reflector->markLazyObjectAsInitialized($obj) === $obj);
var_dump($reflector->isUninitializedLazyObject($obj));

?>
--EXPECT--
Error: Can not reset an object while it is being initialized
bool(true)
bool(false)
