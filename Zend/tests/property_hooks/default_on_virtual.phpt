--TEST--
Virtual properties cannot have default value
--FILE--
<?php

class Test {
    public $prop = 0 {
        get {}
        set {}
    }
}

?>
--EXPECTF--
Fatal error: Cannot specify default value for virtual hooked property Test::$prop in %s on line %d
