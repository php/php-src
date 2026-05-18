--TEST--
Parametric LSP: substitution composes through a generic abstract class and a concrete leaf
--FILE--
<?php
interface Shape<T> {
    public function area(): T;
}

abstract class Polygon<U> implements Shape<U> {}

class IntSquare extends Polygon<int> {
    public function __construct(private int $side) {}
    public function area(): int { return $this->side * $this->side; }
}

echo (new IntSquare(4))->area(), "\n";
?>
--EXPECT--
16
