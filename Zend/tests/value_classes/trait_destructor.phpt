--TEST--
Traits cannot introduce a destructor into a value class
--FILE--
<?php
trait Cleanup { public function __destruct() {} }
value class Foo { use Cleanup; }
?>
--EXPECTF--
Fatal error: Value class Foo cannot include magic method __destruct in %s on line %d
