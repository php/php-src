--TEST--
Associated type behaviour in extended interface
--FILE--
<?php

interface I {
    type T : int|string|(Traversable&Countable);
    public function foo(T $param): T;
}

interface I2 extends I {
    public function bar(int $o, T $param): T;
}

class C implements I2 {
    public function foo(string $param): string {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::bar(int $o, float $param): float must be compatible with I2::bar(int $o, T $param): T in %s on line %d
