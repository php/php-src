--TEST--
Variance: out T on a method's own type parameter in parameter position is rejected
--FILE--
<?php
class A {
    public function f<out T>(T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
