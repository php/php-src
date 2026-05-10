--TEST--
Variance: out T as the type-arg to an invariant trait's parameter is rejected
--FILE--
<?php
trait InvTrait<U> {
    public function rw(U $x): U { return $x; }
}

class A<out T> {
    use InvTrait<T>;
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
