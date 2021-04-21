--TEST--
Unknown accessor (private property)
--FILE--
<?php

class Test {
    private $prop {
        foobar {}
    }
}

?>
--EXPECTF--
Fatal error: Unknown accessor "foobar" for property Test::$prop in %s on line %d
