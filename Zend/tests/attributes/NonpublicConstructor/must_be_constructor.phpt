--TEST--
#[\NonpublicConstructor]: On a private non-constructor
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor]
    private function test() {}
}

?>
--EXPECTF--
Fatal error: #[NonpublicConstructor] can only be applied to constructors in %s on line %d
