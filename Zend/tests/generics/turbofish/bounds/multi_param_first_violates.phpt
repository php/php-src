--TEST--
Turbofish bound check: multi-parameter callee, first arg violates
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

function pair<A : Animal, B : Dog>(): void {}

try {
    pair::<int, Dog>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 1 to call pair() does not satisfy the bound Animal on parameter A, int given
