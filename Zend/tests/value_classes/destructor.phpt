--TEST--
A value class cannot declare a destructor
--FILE--
<?php
value class Foo { public function __destruct() {} }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __destruct in %s on line %d
