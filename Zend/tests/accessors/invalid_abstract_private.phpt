--TEST--
Accessor cannot be both abstract and private
--FILE--
<?php

class Test {
    private $prop {
        abstract get;
    }
}

?>
--EXPECTF--
Fatal error: Accessor cannot be both abstract and private in %s on line %d
