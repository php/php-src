--TEST--
#[\NonInstantiableClass] cannot be applied to trait
--FILE--
<?php

#[\NonInstantiableClass("reason")]
trait Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NonInstantiableClass] to trait Test in %s on line %d
