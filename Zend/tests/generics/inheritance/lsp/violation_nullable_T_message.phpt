--TEST--
Parametric LSP: error message for a ?T return violation shows the substituted nullable form
--FILE--
<?php
class A<T> {
    public function f(): ?T {}
}

class B extends A<int> {
    public function f(): bool { return true; }
}
?>
--EXPECTF--
Fatal error: Declaration of B::f(): bool must be compatible with A::f(): ?int in %s on line %d
