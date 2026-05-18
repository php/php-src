--TEST--
Variance: +T in a hooked property with by-ref get (effectively invariant) is rejected
--FILE--
<?php
class A<+T> {
    private T $backing;
    public function __construct(T $v) { $this->backing = $v; }
    public T $val {
        &get => $this->backing;
    }
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
