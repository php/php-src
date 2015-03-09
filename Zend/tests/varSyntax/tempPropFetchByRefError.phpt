--TEST--
Passing a property fetch on a temporary by reference is not allowed
--FILE--
<?php

$fn = function(&$ref) {};
$fn([0, 1]->prop);

?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
