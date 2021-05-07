--TEST--
Unknown accessor
--FILE--
<?php

class Test {
    public $prop {
        foobar {}
    }
}

?>
--EXPECTF--
Fatal error: Unknown accessor "foobar" for property Test::$prop, expected "get" or "set" in %s on line %d
