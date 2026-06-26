--TEST--
Parametric LSP: diamond inheritance via two generic intermediaries with the same target
--FILE--
<?php
interface Base<T> {
    public function id(T $x): T;
}

interface LeftPath<T> extends Base<T> {}
interface RightPath<T> extends Base<T> {}

class IntDiamond implements LeftPath<int>, RightPath<int> {
    public function id(int $x): int { return $x; }
}

echo (new IntDiamond)->id(99), "\n";
?>
--EXPECT--
99
