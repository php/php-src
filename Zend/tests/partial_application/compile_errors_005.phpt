--TEST--
Partial application compile errors: follow variadic with un-named arg
--FILE--
<?php
foo(..., $a);
?>
--EXPECTF--
Fatal error: Only named arguments may follow variadic placeholder in %s on line %d

