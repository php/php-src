--TEST--
Variance: +T as the type-arg to an invariant interface's parameter is rejected
--FILE--
<?php
interface Inv<U> {}

class A<+T> implements Inv<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
