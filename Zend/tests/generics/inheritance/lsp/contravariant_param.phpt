--TEST--
Parametric LSP: contravariant parameter — child accepts a supertype of the bound argument
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface Eater<T> {
    public function eat(T $x): void;
}

class DogEater implements Eater<Dog> {
    public function eat(Animal $x): void { echo "ate ", get_class($x), "\n"; }
}

(new DogEater)->eat(new Dog);
(new DogEater)->eat(new Animal);
?>
--EXPECT--
ate Dog
ate Animal
