--TEST--
Test fetch nullsafe property by ref
--FILE--
<?php

$foo = null;
$ref = &$foo?->bar

?>
--EXPECTF--
Fatal error: Cannot take reference of a null-safe chain in %s on line %d
