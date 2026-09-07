--TEST--
PFA compile errors: variadic placeholder must be last, including after named args
--FILE--
<?php
foo(..., n: 5);
?>
--EXPECTF--
Fatal error: Variadic placeholder must be last in %s on line %d
