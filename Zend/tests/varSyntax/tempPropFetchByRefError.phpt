--TEST--
Passing a property fetch on a temporary by reference is not allowed
--FILE--
<?php

$fn = function(&$ref) {};
$fn([0, 1]->prop);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use temporary expression in write context in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
