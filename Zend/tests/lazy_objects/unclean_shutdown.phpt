--TEST--
Lazy objects: unclean shutdown
--FILE--
<?php

class C {
    public $a;
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    trigger_error('Fatal', E_USER_ERROR);
});

var_dump($obj->a);
--EXPECTF--
Fatal error: Fatal in %s on line %d
