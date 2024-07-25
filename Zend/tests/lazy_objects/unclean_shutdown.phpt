--TEST--
Lazy objects: unclean shutdown
--FILE--
<?php

class C {
    public $a;
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    trigger_error('Fatal', E_USER_ERROR);
});

var_dump($obj->a);
--EXPECTF--
Fatal error: Fatal in %s on line %d
