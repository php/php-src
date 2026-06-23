--TEST--
#[\NonInstantiableClass] cannot be applied to enum
--FILE--
<?php

#[\NonInstantiableClass("reason")]
enum Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NonInstantiableClass] to enum Test in %s on line %d
