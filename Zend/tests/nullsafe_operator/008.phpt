--TEST--
Test nullsafe property coalesce assignment
--FILE--
<?php

$foo = null;
var_dump($foo?->bar ??= 'bar');

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
