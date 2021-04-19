--TEST--
Partial application compile errors: multiple ...
--FILE--
<?php
foo(..., ...);
?>
--EXPECTF--
Fatal error: Variadic place holder may only appear once in %s on line %d

