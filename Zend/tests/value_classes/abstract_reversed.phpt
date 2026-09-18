--TEST--
A value class cannot be abstract regardless of modifier order
--FILE--
<?php
value abstract class Foo {}
?>
--EXPECTF--
Fatal error: Cannot use the value modifier on an abstract class in %s on line %d
