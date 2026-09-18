--TEST--
Trait aliases cannot introduce a destructor into a value class
--FILE--
<?php
trait Methods { public function cleanup() {} }
value class Foo { use Methods { cleanup as __destruct; } }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __destruct in %s on line %d
