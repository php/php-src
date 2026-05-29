--TEST--
#[\NonInstantiableClass] cannot be applied to function
--FILE--
<?php

#[\NonInstantiableClass("reason")]
function foo() {}

?>
--EXPECTF--
Fatal error: Attribute "NonInstantiableClass" cannot target function (allowed targets: class) in %s on line %d
