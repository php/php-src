--TEST--
Variance: out T appearing in its own bound (invariant position) is rejected
--FILE--
<?php
interface Box<U> { public function get(): U; }

class A<out T : Box<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
