--TEST--
Abstract generic type behaviour in extended interface
--FILE--
<?php

interface I<T : int|string|(Traversable&Countable)> {
    public function foo(T $param): T;
}

interface I2<T : float|bool|stdClass, T2 : int|string|(Traversable&Countable)> extends I<T2> {
    public function bar(T $o, T2 $param): T;
}

class C implements I2<float, string> {
    public function foo(string $param): string {}
    public function bar(float $o, string $param): float {}
}

?>
DONE
--EXPECT--
DONE
