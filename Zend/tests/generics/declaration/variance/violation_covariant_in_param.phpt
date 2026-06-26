--TEST--
Variance: out T in method parameter (contravariant position) is rejected
--FILE--
<?php
class A<out T> {
    public function take(T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
