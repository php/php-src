--TEST--
Test nullsafe property assignment op
--FILE--
<?php

$foo = null;
$foo?->bar += 1;

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
