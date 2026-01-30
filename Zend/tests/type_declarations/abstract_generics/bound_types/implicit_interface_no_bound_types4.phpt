--TEST--
Implicit interface inheritance missing bound types 4
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2 extends I1<string> {
    public function bar(int $o, string $param): string;
}

class C implements I2, I1 {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
}

?>
--EXPECTF--
Fatal error: Interface I1 expects 1 generic parameters, 0 given in %s on line %d
