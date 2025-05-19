--TEST--
Implicit interface inheritance missing bound types 2
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2<T> extends I1<T> {
    public function bar(int $o, T $param): T;
}

class C implements I1, I2<float> {
    public function foo(float $param): float {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Interface I1 expects 1 generic parameters, 0 given in %s on line %d
