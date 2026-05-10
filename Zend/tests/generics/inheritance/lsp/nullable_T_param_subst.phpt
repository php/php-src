--TEST--
Parametric LSP: child param narrower than ?T (substituted to ?int) is rejected
--FILE--
<?php
class A<T> {
    public function take(?T $x): void {}
}

class B extends A<int> {
    public function take(int $x): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of B::take(int $x): void must be compatible with A::take(?int $x): void in %s on line %d
