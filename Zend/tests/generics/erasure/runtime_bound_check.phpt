--TEST--
Erasure: bound is enforced at runtime
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

function f<T : Animal>(T $x): T { return $x; }

echo get_class(f(new Dog)), "\n";

try {
    f("not an animal");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "caught\n";
}
?>
--EXPECT--
Dog
caught
