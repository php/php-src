--TEST--
Variance: self-referenced class name in a violating position is still rejected
--FILE--
<?php
class C<out T> {
    public function take(C<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
