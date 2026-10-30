--TEST--
Property hook list must not be empty
--FILE--
<?php

class Test {
    public $prop {}
}

?>
--EXPECTF--
Fatal error: Property hook list must not be empty in %s on line %d
