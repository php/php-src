--TEST--
A value class cannot be abstract
--FILE--
<?php
abstract value class Foo {}
?>
--EXPECTF--
Fatal error: Cannot use the value modifier on an abstract class in %s on line %d
