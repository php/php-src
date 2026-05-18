--TEST--
Variance: self-referenced class name in a violating position is still rejected
--FILE--
<?php
class C<+T> {
    public function take(C<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
