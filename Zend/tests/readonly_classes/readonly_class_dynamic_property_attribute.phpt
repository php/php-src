--TEST--
Readonly classes cannot apply the #[AllowDynamicProperties] attribute
--FILE--
<?php

#[AllowDynamicProperties]
readonly class Foo
{
}

?>
--EXPECTF--
Fatal error: Cannot apply #[AllowDynamicProperties] to readonly class Foo in %s on line %d
