--TEST--
Variance: in T on a function in return position (covariant) is rejected
--FILE--
<?php
function f<in T>(): T {}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (in T) cannot appear in covariant position in %s on line %d
