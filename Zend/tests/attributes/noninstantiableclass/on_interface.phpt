--TEST--
#[\NonInstantiableClass] cannot be applied to interface
--FILE--
<?php

#[\NonInstantiableClass("reason")]
interface Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NonInstantiableClass] to interface Test in %s on line %d
