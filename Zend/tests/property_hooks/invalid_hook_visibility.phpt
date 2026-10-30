--TEST--
Property hooks cannot have explicitly visibility
--FILE--
<?php

class Test {
    private $prop {
        public get;
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the public modifier on a property hook in %s on line %d
