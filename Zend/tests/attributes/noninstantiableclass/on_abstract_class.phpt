--TEST--
#[\NonInstantiableClass] cannot be applied to abstract class
--FILE--
<?php

#[\NonInstantiableClass("reason")]
abstract class Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NonInstantiableClass] to abstract class Test in %s on line %d
