--TEST--
Implementing class does not bind any abstract generic type
--FILE--
<?php

interface I<T> {
    public function foo(T $param): T;
}

class C implements I {
    public function foo(float $param): float {}
}

?>
--EXPECTF--
Fatal error: Interface I expects 1 generic parameters, 0 given in %s on line %d
