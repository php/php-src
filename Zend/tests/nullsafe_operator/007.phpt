--TEST--
Test nullsafe property pre increment
--FILE--
<?php

$foo = null;
var_dump($foo?->bar++);

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
