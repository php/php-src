--TEST--
#[\NonInstantiableClass] cannot be applied to property
--FILE--
<?php

class Test {
    #[\NonInstantiableClass("reason")]
    public $foo;
}

?>
--EXPECTF--
Fatal error: Attribute "NonInstantiableClass" cannot target property (allowed targets: class) in %s on line %d
