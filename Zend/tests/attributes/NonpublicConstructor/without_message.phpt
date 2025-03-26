--TEST--
#[\NonpublicConstructor]: Message argument is required
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor]
    private function __construct() {}
}

?>
--EXPECTF--
Fatal error: #[NonpublicConstructor] takes 1 parameter, 0 given in %s on line %d
