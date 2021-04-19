--TEST--
Partial application compile errors: follow variadic with un-named arg
--FILE--
<?php
foo(..., $a);
?>
--EXPECTF--
Fatal error: Only named arguments may follow variadic place holder in %s on line %d

