--TEST--
Anonymous value classes are outside the MVP
--FILE--
<?php
new value class {};
?>
--EXPECTF--
Fatal error: Cannot use the value modifier on an anonymous class in %s on line %d
