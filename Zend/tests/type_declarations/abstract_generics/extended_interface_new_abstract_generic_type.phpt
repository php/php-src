--TEST--
Abstract generic type behaviour in extended interface
--FILE--
<?php

interface I<T : int|string|(Traversable&Countable)> {
    public function foo(T $param): T;
}

interface I2<T : int|string|(Traversable&Countable), T2 : float|bool|stdClass> extends I<T> {
    public function bar(T2 $o, T $param): T2;
}

class C implements I2<string, float> {
    public function foo(string $param): string {}
    public function bar(float $o, string $param): float {}
}

?>
DONE
--EXPECT--
DONE
