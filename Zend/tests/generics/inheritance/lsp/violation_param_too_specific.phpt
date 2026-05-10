--TEST--
Parametric LSP: child param more specific than the bound argument is rejected (contravariance)
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface Eater<T> {
    public function eat(T $x): void;
}

class TooSpecificEater implements Eater<Animal> {
    public function eat(Dog $x): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of TooSpecificEater::eat(Dog $x): void must be compatible with Eater::eat(Animal $x): void in %s on line %d
