--TEST--
Lazy objects: unclean shutdown
--FILE--
<?php

class C {
    public $a;
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    // Trigger a fatal error to get an unclean shutdown
    class bool {}
});

var_dump($obj->a);
--EXPECTF--
Fatal error: Cannot use "bool" as a class name%s on line %d
