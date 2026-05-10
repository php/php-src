--TEST--
Variance: -T in a read/write typed property (invariant position) is rejected
--FILE--
<?php
class A<-T> {
    public T $val;
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in invariant position in %s on line %d
