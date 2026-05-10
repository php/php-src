--TEST--
Turbofish bound check: new with a concrete arg satisfying the bound
--FILE--
<?php
class Animal {}
class Dog extends Animal {}
class Box<T : Animal> {}

new Box::<Dog>;
new Box::<Animal>;
echo "OK\n";
?>
--EXPECT--
OK
