--TEST--
Unknown property hook (private property)
--FILE--
<?php

class Test {
    private $prop {
        foobar {}
    }
}

?>
--EXPECTF--
Fatal error: Unknown hook "foobar" for property Test::$prop, expected "get" or "set" in %s on line %d
