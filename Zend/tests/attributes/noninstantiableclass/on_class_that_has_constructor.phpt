--TEST--
#[\NonInstantiableClass] cannot be applied to class that has a constructor
--FILE--
<?php

#[\NonInstantiableClass("reason")]
class Test {
    public function __construct() {}
}

var_dump(new Test());
?>
--EXPECTF--
Fatal error: Cannot apply #[\NonInstantiableClass] to a non-internal class Test in %s on line %d
