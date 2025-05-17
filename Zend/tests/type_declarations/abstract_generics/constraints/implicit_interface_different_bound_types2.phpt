--TEST--
Implicit interface inheritance with different bound types 2
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2<T> extends I1<T> {
    public function bar(int $o, T $param): T;
}

class C implements I1<string>, I2<float> {
    public function foo(string $param): string {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Bound types for implicitly and explicitly implemented interfaces must match in %s on line %d
