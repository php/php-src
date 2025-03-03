--TEST--
#[\NonpublicConstructor]: Message argument must be a string
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor(true)]
    private function __construct() {}
}

?>
--EXPECTF--
Fatal error: NonpublicConstructor::__construct(): Argument #1 ($message) must be of type string, true given in %s on line %d
