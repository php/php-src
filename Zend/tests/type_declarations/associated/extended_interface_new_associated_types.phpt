--TEST--
Associated type behaviour in extended interface
--FILE--
<?php

interface I {
    type T : int|string|(Traversable&Countable);
    public function foo(T $param): T;
}

interface I2 extends I {
    type T2 : float|bool|stdClass;
    public function bar(T2 $o, T $param): T2;
}

class C implements I2 {
    public function foo(string $param): string {}
    public function bar(float $o, string $param): float {}
}

// TODO: Ideally error message would be:
//Fatal error: Declaration of C::bar(float $o, string $param): float must be compatible with I2::bar(<T2 : stdClass|float|bool> $o, <T : (Traversable&Countable)|int|string> $param): <T2 : stdClass|float|bool> in %s on line %d
//Improve zend_append_type_hint()?
?>
--EXPECTF--
Fatal error: Declaration of C::bar(float $o, string $param): float must be compatible with I2::bar(<T2 : stdClass|float|bool> $o, T $param): <T2 : stdClass|float|bool> in %s on line %d
