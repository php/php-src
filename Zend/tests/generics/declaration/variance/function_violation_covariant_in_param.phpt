--TEST--
Variance: +T on a function in parameter position (contravariant) is rejected
--FILE--
<?php
function f<+T>(T $x): void {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
