--TEST--
Parametric LSP: methods on the parent are substituted in the child
--FILE--
<?php
abstract class Factory<T> {
    abstract public function make(): T;
}

class IntFactory extends Factory<int> {
    public function make(): int { return 7; }
}

echo (new IntFactory())->make(), "\n";
?>
--EXPECT--
7
