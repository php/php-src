--TEST--
Variance: out T in a sibling type parameter's default (invariant position) is rejected
--FILE--
<?php
interface Container<U> { public function get(): U; }

class A<out T, U = Container<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
