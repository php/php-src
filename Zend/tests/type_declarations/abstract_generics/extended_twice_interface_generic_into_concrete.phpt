--TEST--
Abstract generic type behaviour in extended interface
--FILE--
<?php

interface I1<T1> {
    public function foo(T1 $param): T1;
}

interface I2<T2> extends I1<T2> {
    public function bar(int $o, T2 $param): T2;
}

interface I3 extends I2<string> {
    public function foobar(string $a, float $b): float;
}

class C implements I3 {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
    public function foobar(string $a, float $b): float {}
}

?>
DONE
--EXPECT--
DONE
