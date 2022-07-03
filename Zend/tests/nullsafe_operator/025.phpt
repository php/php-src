--TEST--
Nullsafe chains in variable variables
--FILE--
<?php

$a = null;
var_dump(${$a?->b}->c);

?>
--EXPECTF--
Warning: Undefined variable $ (This will become an error in PHP 9.0) in %s on line %d

Warning: Attempt to read property "c" on null in %s on line %d
NULL
