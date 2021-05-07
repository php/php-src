--TEST--
Visibility of accessor cannot be higher than of property
--FILE--
<?php

class Test {
    private $prop {
        public get;
    }
}

?>
--EXPECTF--
Fatal error: Visibility of accessor cannot be higher than visibility of property in %s on line %d
