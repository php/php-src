--TEST--
Variance: parent<T> in a contravariant parameter position is rejected for out T
--FILE--
<?php
readonly class A<out T> {
    public function __construct(public T $value) {}
}
class B<out T> extends A<T> {
    public function take(parent<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
