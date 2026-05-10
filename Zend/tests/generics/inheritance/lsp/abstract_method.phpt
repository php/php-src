--TEST--
Parametric LSP: substitution applies to abstract methods inherited from a generic ancestor
--FILE--
<?php
abstract class Container<T> {
    abstract public function unwrap(): T;
}

class IntContainer extends Container<int> {
    public function __construct(private int $v) {}
    public function unwrap(): int { return $this->v; }
}

echo (new IntContainer(11))->unwrap(), "\n";
?>
--EXPECT--
11
