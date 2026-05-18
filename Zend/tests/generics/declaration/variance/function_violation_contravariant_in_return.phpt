--TEST--
Variance: -T on a function in return position (covariant) is rejected
--FILE--
<?php
function f<-T>(): T {}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in covariant position in %s on line %d
