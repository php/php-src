--TEST--
Variance: out T in a read/write typed property (invariant position) is rejected
--FILE--
<?php
class A<out T> {
    public T $val;
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
