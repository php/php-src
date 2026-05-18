--TEST--
Turbofish bound check: new with a concrete arg violating the bound throws TypeError
--FILE--
<?php
class Animal {}
class Box<T : Animal> {}

try {
    new Box::<int>;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 1 to new Box does not satisfy the bound Animal on parameter T, int given
