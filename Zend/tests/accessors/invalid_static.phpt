--TEST--
Accessor method cannot be static
--FILE--
<?php

class Test {
    public $prop {
        static get {}
    }
}

?>
--EXPECTF--
Fatal error: Accessor cannot be static in %s on line %d
