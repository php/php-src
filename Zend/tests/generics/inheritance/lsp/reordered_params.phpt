--TEST--
Parametric LSP: child reorders the parent's type parameters
--FILE--
<?php
interface Pair<A, B> {
    public function first(): A;
    public function second(): B;
}

class IntStrPair implements Pair<int, string> {
    public function __construct(private int $a, private string $b) {}
    public function first(): int { return $this->a; }
    public function second(): string { return $this->b; }
}

class FlippedPair<X, Y> implements Pair<Y, X> {
    public function __construct(private mixed $x, private mixed $y) {}
    public function first(): Y { return $this->y; }
    public function second(): X { return $this->x; }
}

class StrIntFlipped extends FlippedPair<int, string> {}

$p = new IntStrPair(1, "hi");
echo $p->first(), "/", $p->second(), "\n";

$f = new StrIntFlipped(7, "world");
echo $f->first(), "/", $f->second(), "\n";
?>
--EXPECT--
1/hi
world/7
