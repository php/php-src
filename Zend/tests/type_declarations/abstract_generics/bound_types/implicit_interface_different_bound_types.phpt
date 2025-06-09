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
Fatal error: Bound type T for interface I1 implemented explicitly in C with type string must match the implicitly bound type float from interface I2 in %s on line %d
