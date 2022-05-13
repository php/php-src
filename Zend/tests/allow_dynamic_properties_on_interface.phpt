--TEST--
#[AllowDynamicProperties] cannot be applied to interface
--FILE--
<?php

#[AllowDynamicProperties]
interface Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[AllowDynamicProperties] to interface in %s on line %d
