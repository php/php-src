--TEST--
A value class cannot extend a readonly class
--FILE--
<?php
readonly class ParentClass {}
value class Foo extends ParentClass {}
?>
--EXPECTF--
Fatal error: Value class Foo cannot extend another class in %s on line %d
