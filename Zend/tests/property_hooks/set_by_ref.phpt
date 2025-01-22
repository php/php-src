--TEST--
set parameter must not be by-reference
--FILE--
<?php

class Test {
    public $prop {
        set(&$value) {}
    }
}

?>
--EXPECTF--
Fatal error: Parameter $value of set hook Test::$prop must not be pass-by-reference in %s on line %d
