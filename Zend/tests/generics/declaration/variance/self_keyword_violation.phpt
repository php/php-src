--TEST--
Variance: self<T> in a contravariant parameter position is rejected for out T
--FILE--
<?php
class C<out T> {
    public function take(self<T> $other): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
