--TEST--
A value class cannot extend an ordinary class
--FILE--
<?php
class ParentClass {}
value class Foo extends ParentClass {}
?>
--EXPECTF--
Fatal error: Value class Foo cannot extend another class in %s on line %d
