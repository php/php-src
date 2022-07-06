--TEST--
::class on an expression cannot be used inside constant expressions
--FILE--
<?php

const A = [0]::class;

?>
--EXPECTF--
Fatal error: (expression)::class cannot be used in constant expressions in %s on line %d
