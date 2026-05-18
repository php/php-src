--TEST--
Parametric LSP: ?T return substitutes through a multi-level chain (interface extending interface)
--FILE--
<?php
interface I<T> {
    public function next(): ?T;
}

interface J<U> extends I<U> {}

class C implements J<int> {
    public function next(): ?int { return 7; }
}

var_dump((new C)->next());
?>
--EXPECT--
int(7)
