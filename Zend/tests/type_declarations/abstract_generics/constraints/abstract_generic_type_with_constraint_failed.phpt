--TEST--
Abstract generic type with a constraint that is not satisfied
--FILE--
<?php

interface I<T : int|string> {
    public function foo(T $param): T;
}

class C implements I<float> {
    public function foo(float $param): float {}
}

?>
--EXPECTF--
Fatal error: Bound type float is not a subtype of the constraint type string|int of generic type T of interface I in %s on line %d
