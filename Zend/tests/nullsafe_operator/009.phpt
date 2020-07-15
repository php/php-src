--TEST--
Test fetch nullsafe property by ref
--FILE--
<?php

$foo = null;
$ref = &$foo?->bar

?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s.php on line 4
