--TEST--
Set hook value param must not have a default value
--FILE--
<?php

class B {
    public $x {
        set($value = null) {
            $field = $value;
        }
    }
}

?>
--EXPECTF--
Fatal error: Parameter $value of set hook B::$x must not have a default value in %s on line %d
