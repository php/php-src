--TEST--
A value class cannot declare a clone method
--FILE--
<?php
value class Foo { public function __CLONE() {} }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __clone in %s on line %d
