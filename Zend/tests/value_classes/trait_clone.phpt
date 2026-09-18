--TEST--
Traits cannot introduce a clone method into a value class
--FILE--
<?php
trait Copy { public function __clone() {} }
value class Foo { use Copy; }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __clone in %s on line %d
