--TEST--
set parameter must not be variadic
--FILE--
<?php

class Test {
    public $prop {
        set(...$value) {}
    }
}

?>
--EXPECTF--
Fatal error: Parameter $value of set hook Test::$prop must not be variadic in %s on line %d
