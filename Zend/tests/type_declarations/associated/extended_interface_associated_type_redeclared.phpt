--TEST--
Associated type behaviour in extended interface
--FILE--
<?php

interface I {
    type T : int|string;
    public function foo(T $param): T;
}

interface I2 extends I {
    type T;
    public function bar(int $o, T $param): T;
}

class C implements I2 {
    public function foo(string $param): string {}
    public function bar(int $o, float $param): float {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare associated type T in interface I2 inherited from interface I in %s on line %d
