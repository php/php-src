--TEST--
#[\NonpublicConstructor]: On a public constructor
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor]
    public function __construct() {}
}

?>
--EXPECTF--
Fatal error: #[NonpublicConstructor] can only be applied to protected or private constructors in %s on line %d
