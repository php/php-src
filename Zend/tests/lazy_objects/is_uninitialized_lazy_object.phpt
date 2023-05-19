--TEST--
Lazy objects: isUninitializedLazyObject()
--FILE--
<?php

class C {
    public int $a;

    public function __construct() {
        $this->a = 1;
    }
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function () {});
var_dump($reflector->isUninitializedLazyObject($obj));
$reflector->initializeLazyObject($obj);
var_dump($reflector->isUninitializedLazyObject($obj));

$obj = $reflector->newLazyProxy(function () { return new C(); });
var_dump($reflector->isUninitializedLazyObject($obj));
$reflector->initializeLazyObject($obj);
var_dump($reflector->isUninitializedLazyObject($obj));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
