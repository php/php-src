--TEST--
set accessor parameter cannot be variadic
--FILE--
<?php

class Test {
    public $prop {
        set(...$value) {}
    }
}

?>
--EXPECTF--
Fatal error: Accessor "set" must have exactly one required by-value parameter in %s on line %d
