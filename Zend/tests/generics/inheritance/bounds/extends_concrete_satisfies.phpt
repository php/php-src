--TEST--
Bound check: class extends with a concrete arg satisfying the parent bound
--FILE--
<?php
class Animal {}
class Dog extends Animal {}
class Box<T : Animal> {}

class DogBox extends Box<Dog> {}
echo "OK\n";
?>
--EXPECT--
OK
