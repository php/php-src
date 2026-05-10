--TEST--
Variance: +T as the type-arg to an invariant trait's parameter is rejected
--FILE--
<?php
trait InvTrait<U> {
    public function rw(U $x): U { return $x; }
}

class A<+T> {
    use InvTrait<T>;
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
