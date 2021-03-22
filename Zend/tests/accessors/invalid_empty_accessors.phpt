--TEST--
Accessor list cannot be empty
--FILE--
<?php

class Test {
    public $prop {}
}

?>
--EXPECTF--
Fatal error: Accessor list cannot be empty in %s on line %d
