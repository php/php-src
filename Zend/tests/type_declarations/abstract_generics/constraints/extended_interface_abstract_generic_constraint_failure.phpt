--TEST--
Abstract generic type behaviour in extended interface violates type constraint
--FILE--
<?php

interface I<T : int|string|(Traversable&Countable)> {
    public function foo(T $param): T;
}

interface I2<T> extends I<T> {
    public function bar(int $o, T $param): T;
}

class C implements I2<string> {
    public function foo(string $param): string {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Constraint type mixed of generic type T of interface I2 is not a subtype of the constraint type (Traversable&Countable)|string|int of generic type T of interface I in %s on line %d
