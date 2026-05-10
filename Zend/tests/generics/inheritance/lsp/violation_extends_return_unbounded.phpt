--TEST--
Parametric LSP: child of `extends A<int>` cannot override return type T with an unrelated type
--FILE--
<?php
class A<T> {
    public function get(): T {}
}

class IntChild extends A<int> {
    public function get(): bool { return true; }
}
?>
--EXPECTF--
Fatal error: Declaration of IntChild::get(): bool must be compatible with A::get(): int in %s on line %d
