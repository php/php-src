--TEST--
GH-20113: First-class callable constructor in constant AST
--FILE--
<?php
const C = new \stdClass(...);
?>
--EXPECTF--
Fatal error: Cannot create Closure for new expression in %s on line %d
