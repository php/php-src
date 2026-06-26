--TEST--
Variance: out T on a function in parameter position (contravariant) is rejected
--FILE--
<?php
function f<out T>(T $x): void {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
