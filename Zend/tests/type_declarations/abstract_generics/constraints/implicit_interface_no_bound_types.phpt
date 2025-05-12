--TEST--
Implicit interface inheritance missing bound types
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
Fatal error: Bound type float is not a subtype of the constraint type string|int of generic type T of interface I in %s on line %d
