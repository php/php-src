--TEST--
Visibility of accessor cannot be (explicitly) same as property
--FILE--
<?php

class Test {
    protected $prop {
        protected get;
    }
}

?>
--EXPECTF--
Fatal error: Explicit accessor visibility cannot be the same as property visibility. Omit the explicit accessor visibility in %s on line %d
