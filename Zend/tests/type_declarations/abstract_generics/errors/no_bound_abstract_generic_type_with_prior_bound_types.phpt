--TEST--
Implementing class does not bind any abstract generic type
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}
interface I2<T> {
    public function bar(T $param): T;
}

class C implements I1<float>, I2 {
    public function foo(float $param): float {}
    public function bar(string $param): string {}
}

?>
--EXPECTF--
Fatal error: Interface I2 expects 1 generic parameters, 0 given in %s on line %d
