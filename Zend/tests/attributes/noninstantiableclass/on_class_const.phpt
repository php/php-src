--TEST--
#[\NonInstantiableClass] cannot be applied to class constant
--FILE--
<?php

class Test {
    #[\NonInstantiableClass("reason")]
    const FOO = 5;
}

?>
--EXPECTF--
Fatal error: Attribute "NonInstantiableClass" cannot target class constant (allowed targets: class) in %s on line %d
