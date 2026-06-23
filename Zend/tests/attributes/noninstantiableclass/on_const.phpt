--TEST--
#[\NonInstantiableClass] cannot be applied to constant
--FILE--
<?php

#[\NonInstantiableClass("reason")]
const FOO = 5;

?>
--EXPECTF--
Fatal error: Attribute "NonInstantiableClass" cannot target constant (allowed targets: class) in %s on line %d
