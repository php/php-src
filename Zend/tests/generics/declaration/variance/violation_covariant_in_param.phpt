--TEST--
Variance: +T in method parameter (contravariant position) is rejected
--FILE--
<?php
class A<+T> {
    public function take(T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
