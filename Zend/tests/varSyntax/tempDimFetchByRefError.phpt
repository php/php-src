--TEST--
Passing a dimension fetch on a temporary by reference is not allowed
--FILE--
<?php

$fn = function(&$ref) {};
$fn([0, 1][0]);

?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
