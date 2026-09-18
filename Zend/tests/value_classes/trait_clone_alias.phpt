--TEST--
Trait aliases cannot introduce a clone method into a value class
--FILE--
<?php
trait Methods { public function copy() {} }
value class Foo { use Methods { copy as __clone; } }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __clone in %s on line %d
