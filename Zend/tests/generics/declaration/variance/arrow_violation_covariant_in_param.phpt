--TEST--
Variance: out T on an arrow function in parameter position is rejected
--FILE--
<?php
$f = fn<out T>(T $x): T => $x;
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
