--TEST--
Variance: +T mentioned in a sibling type parameter's bound (invariant position) is rejected
--FILE--
<?php
interface Container<U> { public function get(): U; }

class A<+T, U : Container<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
