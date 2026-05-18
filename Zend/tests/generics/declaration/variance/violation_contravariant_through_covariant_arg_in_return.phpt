--TEST--
Variance: -T composed through Producer<+U> in return (+ × + = +) is rejected
--FILE--
<?php
interface Producer<+U> { public function produce(): U; }

class A<-T> {
    public function makeProducer(): Producer<T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in covariant position in %s on line %d
