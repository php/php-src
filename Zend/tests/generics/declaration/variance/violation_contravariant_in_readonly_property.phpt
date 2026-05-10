--TEST--
Variance: -T in a readonly property (covariant position) is rejected
--FILE--
<?php
class A<-T> {
    public readonly T $val;
    public function __construct(T $v) { $this->val = $v; }
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in covariant position in %s on line %d
