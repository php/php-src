--TEST--
Inheritance: self / static in a generic ancestor compose with parametric LSP
--FILE--
<?php
abstract class Builder<T> {
    abstract public function build(): T;
    public static function make(): static { return new static; }
}

class IntBuilder extends Builder<int> {
    public function build(): int { return 42; }
}

interface Box<T> {
    public function unwrap(): T;
    public function self_clone(): self;
}

class IntBox implements Box<int> {
    public function __construct(private int $v) {}
    public function unwrap(): int { return $this->v; }
    public function self_clone(): self { return new self($this->v); }
}

$ib = IntBuilder::make();
echo get_class($ib), ":", $ib->build(), "\n";

$b = new IntBox(7);
echo $b->unwrap(), "\n";
$c = $b->self_clone();
echo get_class($c), ":", $c->unwrap(), "\n";
?>
--EXPECT--
IntBuilder:42
7
IntBox:7
