--TEST--
GH-20174: Assertion failure in ReflectionProperty::skipLazyInitialization after failed LazyProxy skipLazyInitialization
--CREDITS--
vi3tL0u1s
--FILE--
<?php

class C {
    public $b;
    public $c;
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyProxy(function ($obj) {
    $obj->b = 4;
    throw new Exception();
});

try {
    $reflector->initializeLazyObject($obj);
} catch (Exception $e) {
    $reflector->getProperty('b')->skipLazyInitialization($obj);
}

var_dump($obj);

?>
--EXPECTF--
lazy proxy object(C)#%d (1) {
  ["b"]=>
  NULL
}
