--TEST--
Property hook cannot be static
--FILE--
<?php

class Test {
    public $prop {
        static get {}
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the static modifier on a property hook in %s on line %d
