--TEST--
Variance: in T in method return (covariant position) is rejected
--FILE--
<?php
class A<in T> {
    public function get(): T {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (in T) cannot appear in covariant position in %s on line %d
