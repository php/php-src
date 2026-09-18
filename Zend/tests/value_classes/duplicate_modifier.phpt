--TEST--
The value class modifier cannot be repeated
--FILE--
<?php
value value class Foo {}
?>
--EXPECTF--
Fatal error: Multiple value modifiers are not allowed in %s on line %d
