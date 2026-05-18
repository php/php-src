--TEST--
Variance: +T as the type-arg to an invariant parent's parameter is rejected
--FILE--
<?php
class Inv<U> {}

class A<+T> extends Inv<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
