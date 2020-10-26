--TEST--
Test nullsafe as foreach target
--FILE--
<?php

$foo = null;
foreach ([1, 2, 3] as $foo?->bar) {}

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
