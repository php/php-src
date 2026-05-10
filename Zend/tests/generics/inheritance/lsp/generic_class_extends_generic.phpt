--TEST--
Parametric LSP: a generic class extending a generic class forwards bindings to subclasses
--FILE--
<?php
class A<X> {
    public function f(X $x): void {}
}

class B<Y> extends A<Y> {}

class C extends B<int> {
    public function f(int $x): void { echo "C::f($x)\n"; }
}

(new C)->f(7);
?>
--EXPECT--
C::f(7)
