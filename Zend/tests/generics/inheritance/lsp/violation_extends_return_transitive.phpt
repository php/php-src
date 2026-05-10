--TEST--
Parametric LSP: violation propagates through a two-level extends chain
--FILE--
<?php
abstract class A<T> {
    abstract public function get(): T;
}

abstract class B<U> extends A<U> {}

class C extends B<int> {
    public function get(): bool { return true; }
}
?>
--EXPECTF--
Fatal error: Declaration of C::get(): bool must be compatible with A::get(): int in %s on line %d
