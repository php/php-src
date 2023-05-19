--TEST--
Lazy objects: newInstanceLazy can instantiate sub-class of user classes
--FILE--
<?php

class B {}
class C extends B {}

foreach (['resetAsLazyGhost', 'resetAsLazyProxy'] as $strategy) {
    $reflector = new ReflectionClass(C::class);
    $obj = $reflector->newInstanceWithoutConstructor();
    $reflector->$strategy($obj, function ($obj) {
        var_dump("initializer");
    });
    var_dump($obj);
}

--EXPECTF--
object(C)#%d (0) {
}
object(C)#%d (0) {
}
