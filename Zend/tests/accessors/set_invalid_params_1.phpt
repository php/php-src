--TEST--
set accessor must have exactly one parameter
--FILE--
<?php

class Test {
    public $prop {
        set() {}
    }
}

?>
--EXPECTF--
Fatal error: Accessor "set" must have exactly one required by-value parameter in %s on line %d
