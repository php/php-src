--TEST--
set $value parameter variance
--FILE--
<?php

interface X {}
interface Y extends X {}

class A {
    public Y $prop {
        set(X $prop) {}
    }
}

class B extends A {
    public Y $prop {
        set(Y $prop) {}
    }
}

?>
--EXPECTF--
Fatal error: Declaration of B::$prop::set(Y $prop): void must be compatible with A::$prop::set(X $prop): void in %s on line %d
