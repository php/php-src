--TEST--
Callable conversion of parent hook call
--FILE--
<?php

class B {
    public mixed $x;
}
class C extends B {
    public mixed $x {
        set {
            $f = parent::$x::set(...);
            $f($value);
        }
    }
}

$c = new C();
$c->x = 0;

?>
--EXPECTF--
Fatal error: Cannot create Closure for parent property hook call in %s on line %d
