--TEST--
Partial application compile errors: only named arguments after ...
--FILE--
<?php
foo(..., ?);
?>
--EXPECTF--
Fatal error: Only named arguments may follow variadic place holder in %s on line %d

