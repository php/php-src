--TEST--
Parametric LSP: covariant return — child returns a strict subtype of the bound argument
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface Producer<T> {
    public function produce(): T;
}

class DogProducer implements Producer<Animal> {
    public function produce(): Dog { return new Dog; }
}

echo get_class((new DogProducer)->produce()), "\n";
?>
--EXPECT--
Dog
