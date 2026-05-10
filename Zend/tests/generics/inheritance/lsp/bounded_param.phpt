--TEST--
Parametric LSP: substitution still composes with a bounded type parameter
--FILE--
<?php
class Animal { public function name(): string { return "?"; } }
class Dog extends Animal { public function name(): string { return "dog"; } }

interface Bag<T : Animal> {
    public function add(T $x): void;
}

class DogBag implements Bag<Dog> {
    public function add(Dog $x): void { echo "added ", $x->name(), "\n"; }
}

(new DogBag)->add(new Dog);
?>
--EXPECT--
added dog
