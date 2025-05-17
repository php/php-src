--TEST--
Implicit interface inheritance missing bound types
--XFAIL--
Wrong number of missing params
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2<T> extends I1<T> {
    public function bar(int $o, T $param): T;
}

class C implements I2<float>, I1 {
    public function foo(float $param): float {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Cannot implement I1 as the number of generic arguments specified (0) does not match the number of generic parameters declared on the interface (1) in %s on line %d
