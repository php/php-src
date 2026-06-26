--TEST--
Turbofish bound check: multi-parameter callee, second arg violates
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

function pair<A : Animal, B : Dog>(): void {}

try {
    pair::<Dog, Animal>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 2 to call pair() does not satisfy the bound Dog on parameter B, Animal given
