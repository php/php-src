--TEST--
Implicit interface inheritance with different bound types 4
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2 extends I1<float> {
    public function bar(int $o, float $param): float;
}

class C implements I1<string>, I2 {
    public function foo(string $param): string {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Bound type T for interface I1 implemented explicitly in C with type string must match the implicitly bound type float from interface I2 in %s on line %d
