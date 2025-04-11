--TEST--
Associated type with a constraint that is not satisfied
--FILE--
<?php

interface I {
    type T : int|string;
    public function foo(T $param): T;
}

class C implements I {
    public function foo(float $param): float {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::foo(float $param): float must be compatible with I::foo(T<string|int> $param): T<string|int> in %s on line %d
