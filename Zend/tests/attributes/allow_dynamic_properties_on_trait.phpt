--TEST--
#[AllowDynamicProperties] cannot be applied to trait
--FILE--
<?php

#[AllowDynamicProperties]
trait Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[AllowDynamicProperties] to trait Test in %s on line %d
