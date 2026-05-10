--TEST--
Parametric LSP: a method-level generic parameter is not affected by class-level substitution
--FILE--
<?php
interface Container<T> {
    public function get(): T;
    public function map<U>(callable $f): U;
}

class IntContainer implements Container<int> {
    public function get(): int { return 5; }
    public function map<U>(callable $f): U { return $f(42); }
}

echo (new IntContainer)->get(), "\n";
echo (new IntContainer)->map(fn($x) => "v=$x"), "\n";
?>
--EXPECT--
5
v=42
