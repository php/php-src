--TEST--
Variance: +T in a sibling type parameter's default (invariant position) is rejected
--FILE--
<?php
interface Container<U> { public function get(): U; }

class A<+T, U = Container<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
