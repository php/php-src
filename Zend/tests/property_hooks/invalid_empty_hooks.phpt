--TEST--
Property hook list cannot be empty
--FILE--
<?php

class Test {
    public $prop {}
}

?>
--EXPECTF--
Fatal error: Property hook list cannot be empty in %s on line %d
