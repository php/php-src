--TEST--
#[\NonInstantiableClass] cannot be applied to method
--FILE--
<?php

class Test {
    #[\NonInstantiableClass("reason")]
    public function foo() {}
}

?>
--EXPECTF--
Fatal error: Attribute "NonInstantiableClass" cannot target method (allowed targets: class) in %s on line %d
