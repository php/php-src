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
Fatal error: Cannot implement I as it has generic parameters which are not specified in %s on line %d
