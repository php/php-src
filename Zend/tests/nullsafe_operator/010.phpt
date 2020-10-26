--TEST--
Test fetch nested nullsafe property by ref
--FILE--
<?php

$foo = null;
$ref = &$foo?->bar->baz;

?>
--EXPECTF--
Fatal error: Cannot take reference of a null-safe chain in %s on line %d
