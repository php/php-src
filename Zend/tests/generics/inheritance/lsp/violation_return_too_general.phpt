--TEST--
Parametric LSP: child return more general than the bound argument is rejected (covariance)
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface Producer<T> {
    public function produce(): T;
}

class TooGeneralProducer implements Producer<Dog> {
    public function produce(): Animal { return new Animal; }
}
?>
--EXPECTF--
Fatal error: Declaration of TooGeneralProducer::produce(): Animal must be compatible with Producer::produce(): Dog in %s on line %d
