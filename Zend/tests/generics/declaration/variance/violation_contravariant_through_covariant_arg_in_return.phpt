--TEST--
Variance: in T composed through Producer<out U> in return (+ × + = +) is rejected
--FILE--
<?php
interface Producer<out U> { public function produce(): U; }

class A<in T> {
    public function makeProducer(): Producer<T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (in T) cannot appear in covariant position in %s on line %d
