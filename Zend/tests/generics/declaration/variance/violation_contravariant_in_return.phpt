--TEST--
Variance: -T in method return (covariant position) is rejected
--FILE--
<?php
class A<-T> {
    public function get(): T {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in covariant position in %s on line %d
