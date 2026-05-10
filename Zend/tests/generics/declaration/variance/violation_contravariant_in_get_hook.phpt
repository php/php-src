--TEST--
Variance: -T in a hooked property with set hook (invariant position) is rejected
--FILE--
<?php
class A<-T> {
    private T $backing;
    public function __construct(T $v) { $this->backing = $v; }
    public T $val {
        get => $this->backing;
        set(T $v) { $this->backing = $v; }
    }
}
?>
--EXPECTF--
Fatal error: Type parameter T declared contravariant (-T) cannot appear in invariant position in %s on line %d
