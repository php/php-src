--TEST--
Variance: +T appearing in its own bound (invariant position) is rejected
--FILE--
<?php
interface Box<U> { public function get(): U; }

class A<+T : Box<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
