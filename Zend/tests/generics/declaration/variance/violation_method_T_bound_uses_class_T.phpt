--TEST--
Variance: out T appearing inside a method-level type parameter's bound (invariant) is rejected
--FILE--
<?php
interface Container<U> {}

class A<out T> {
    public function map<W : Container<T>>(W $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
