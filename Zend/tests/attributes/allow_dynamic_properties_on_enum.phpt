--TEST--
#[AllowDynamicProperties] cannot be applied to enum (GH-15731)
--FILE--
<?php

#[AllowDynamicProperties]
enum Test {}

?>
--EXPECTF--
Fatal error: Cannot apply #[AllowDynamicProperties] to enum Test in %s on line %d
