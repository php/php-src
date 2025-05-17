--TEST--
Implicit interface inheritance with different bound types
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2<T> extends I1<T> {
    public function bar(int $o, T $param): T;
}

class C implements I2<float>, I1<string> {
    public function foo(float $param): float {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Bound types for implicitly and explicitly implemented interfaces must match in %s on line %d
