--TEST--
Variance: out T on a closure in parameter position is rejected
--FILE--
<?php
$f = function <out T>(T $x): void {};
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
