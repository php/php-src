--TEST--
Variance: out T composed through Consumer<in U> in return (+ × - = -) is rejected
--FILE--
<?php
interface Consumer<in U> { public function consume(U $x): void; }

class A<out T> {
    public function makeConsumer(): Consumer<T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
