--TEST--
AllowDynamicProperties cannot override value class semantics
--FILE--
<?php
#[AllowDynamicProperties]
value class Foo {}
?>
--EXPECTF--
Fatal error: Cannot apply #[\AllowDynamicProperties] to readonly class Foo in %s on line %d
