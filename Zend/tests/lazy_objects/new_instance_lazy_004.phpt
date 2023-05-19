--TEST--
Lazy objects: newInstanceLazy can instantiate stdClass
--FILE--
<?php

foreach (['resetAsLazyGhost', 'resetAsLazyProxy'] as $strategy) {
    $reflector =new ReflectionClass(stdClass::class);
    $obj = $reflector->newInstanceWithoutConstructor();
    $reflector->$strategy($obj, function ($obj) {
        var_dump("initializer");
    });
    var_dump($obj);
}
--EXPECTF--
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
