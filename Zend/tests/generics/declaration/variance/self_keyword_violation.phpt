--TEST--
Variance: self<T> in a contravariant parameter position is rejected for +T
--FILE--
<?php
class C<+T> {
    public function take(self<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
