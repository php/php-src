--TEST--
set accessor parameter cannot have parameter type
--FILE--
<?php

class Test {
    public int $prop {
        set(int $value) {}
    }
}

?>
--EXPECTF--
Fatal error: Accessor "set" may not have a parameter type (accessor types are determined by the property type) in %s on line %d
