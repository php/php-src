--TEST--
Pipe assign operator error: unparenthesized arrow function
--FILE--
<?php

$x = 5;
$x |>= fn($v) => $v * 2;

?>
--EXPECTF--
Fatal error: Arrow functions on the right hand side of |>= must be parenthesized in %s on line %d
