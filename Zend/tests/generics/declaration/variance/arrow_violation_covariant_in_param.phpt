--TEST--
Variance: +T on an arrow function in parameter position is rejected
--FILE--
<?php
$f = fn<+T>(T $x): T => $x;
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
