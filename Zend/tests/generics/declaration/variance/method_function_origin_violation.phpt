--TEST--
Variance: +T on a method's own type parameter in parameter position is rejected
--FILE--
<?php
class A {
    public function f<+T>(T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
