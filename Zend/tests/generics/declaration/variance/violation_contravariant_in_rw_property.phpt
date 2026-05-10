--TEST--
Variance: in T in a read/write typed property (invariant position) is rejected
--FILE--
<?php
class A<in T> {
    public T $val;
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (in T) cannot appear in invariant position in %s on line %d
