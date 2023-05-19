--TEST--
Lazy objects: ReflectionClass::isInitialized
--FILE--
<?php

class C {
    public int $a;
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->a = 1;
});

$reflector = new ReflectionClass($obj);
var_dump(!$reflector->isUninitializedLazyObject($obj));

var_dump($obj->a);

var_dump(!$reflector->isUninitializedLazyObject($obj));
--EXPECTF--
bool(false)
string(11) "initializer"
int(1)
bool(true)
