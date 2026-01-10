--TEST--
GH-20113: new Foo(...) error in constant expressions
--FILE--
<?php
const C = new \stdClass(...);
?>
--EXPECTF--
Fatal error: Cannot create Closure for new expression in %s on line %d
