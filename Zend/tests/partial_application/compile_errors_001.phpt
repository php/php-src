--TEST--
PFA compile errors: multiple variadic placeholders
--FILE--
<?php
foo(..., ...);
?>
--EXPECTF--
Fatal error: Variadic placeholder may only appear once in %s on line %d
