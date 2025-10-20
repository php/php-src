--TEST--
#[\NoSerialize]: Interface.
--FILE--
<?php

#[NoSerialize]
interface I {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NoSerialize] to interface I in %s on line %d
