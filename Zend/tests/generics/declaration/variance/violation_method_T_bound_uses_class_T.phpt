--TEST--
Variance: +T appearing inside a method-level type parameter's bound (invariant) is rejected
--FILE--
<?php
interface Container<U> {}

class A<+T> {
    public function map<W : Container<T>>(W $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
