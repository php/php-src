--TEST--
Parametric LSP: abstract generic class extended by another abstract class, then concretized
--FILE--
<?php
abstract class Base<T> {
    abstract public function process(T $x): T;
}

abstract class Middle<U> extends Base<U> {
    public function double(U $x): U { return $this->process($this->process($x)); }
}

class IntLeaf extends Middle<int> {
    public function process(int $x): int { return $x + 1; }
}

echo (new IntLeaf)->double(10), "\n";
?>
--EXPECT--
12
