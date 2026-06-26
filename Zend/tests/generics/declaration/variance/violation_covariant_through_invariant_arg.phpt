--TEST--
Variance: out T composed through an invariant generic's slot (invariant) is rejected
--FILE--
<?php
interface Inv<U> { public function rw(U $x): U; }

class A<out T> {
    public function get(): Inv<T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
