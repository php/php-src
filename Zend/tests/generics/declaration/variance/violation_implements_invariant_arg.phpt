--TEST--
Variance: out T as the type-arg to an invariant interface's parameter is rejected
--FILE--
<?php
interface Inv<U> {}

class A<out T> implements Inv<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
