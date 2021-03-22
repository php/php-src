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
Fatal error: Unknown accessor "foobar" for property Test::$prop in %s on line 5
