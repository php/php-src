--TEST--
PFA compile errors: variadic placeholder must be last, including after positional args
--FILE--
<?php
foo(..., $a);
?>
--EXPECTF--
Fatal error: Variadic placeholder must be last in %s on line %d
