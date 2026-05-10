--TEST--
Parametric LSP: ?T return type substitutes through implements with the nullable bit preserved
--FILE--
<?php
interface A<T> {
    public function f(): ?T;
}

class B implements A<int> {
    public function f(): ?int { return 1; }
}

var_dump((new B)->f());
?>
--EXPECT--
int(1)
