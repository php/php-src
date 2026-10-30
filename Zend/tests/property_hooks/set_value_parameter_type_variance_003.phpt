--TEST--
set $value parameter variance
--FILE--
<?php

class Test {
    public X $prop {
        set(Y $prop) {}
    }
}

interface X {}
interface Y {}

?>
--EXPECTF--
Fatal error: Type of parameter $prop of hook Test::$prop::set must be compatible with property type in %s on line %d
