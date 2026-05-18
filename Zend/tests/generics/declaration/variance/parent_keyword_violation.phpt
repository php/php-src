--TEST--
Variance: parent<T> in a contravariant parameter position is rejected for +T
--FILE--
<?php
readonly class A<+T> {
    public function __construct(public T $value) {}
}
class B<+T> extends A<T> {
    public function take(parent<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
