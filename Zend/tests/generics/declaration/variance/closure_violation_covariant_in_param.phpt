--TEST--
Variance: +T on a closure in parameter position is rejected
--FILE--
<?php
$f = function <+T>(T $x): void {};
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
