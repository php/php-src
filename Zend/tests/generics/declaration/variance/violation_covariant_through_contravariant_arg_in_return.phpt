--TEST--
Variance: +T composed through Consumer<-U> in return (+ × - = -) is rejected
--FILE--
<?php
interface Consumer<-U> { public function consume(U $x): void; }

class A<+T> {
    public function makeConsumer(): Consumer<T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
