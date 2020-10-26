--TEST--
Test nullsafe in unset
--FILE--
<?php

$foo = null;
unset($foo?->bar->baz);

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
