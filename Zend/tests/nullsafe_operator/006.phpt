--TEST--
Test nullsafe property post increment
--FILE--
<?php

$foo = null;
++$foo?->bar;

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
