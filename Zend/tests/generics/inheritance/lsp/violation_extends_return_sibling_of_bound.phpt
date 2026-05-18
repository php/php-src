--TEST--
Parametric LSP: child of `extends A<Dog>` cannot override return type T with a sibling of the bound (Cat)
--FILE--
<?php
class Animal {}
class Dog extends Animal {}
class Cat extends Animal {}

class A<T: Animal> {
    public function get(): T {}
}

class DogChild extends A<Dog> {
    public function get(): Cat { return new Cat(); }
}
?>
--EXPECTF--
Fatal error: Declaration of DogChild::get(): Cat must be compatible with A::get(): Dog in %s on line %d
