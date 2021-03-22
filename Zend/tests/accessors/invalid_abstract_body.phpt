--TEST--
Abstract accessor cannot have body
--FILE--
<?php

class Test {
    public $prop {
        abstract get {}
    }
}

?>
--EXPECTF--
Fatal error: Abstract accessor cannot have body in %s on line %d
