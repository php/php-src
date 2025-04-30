--TEST--
Override static with class that is even not an implementation of interface and add a type that is not in the interface in final class
--FILE--
<?php

interface A
{
    public function methodScalar1(): static|bool;
}

final class C
{
}

final class B implements A
{
    public function methodScalar1(): C|array { return []; }
}

$b = new B();
var_dump($b->methodScalar1());
?>
--EXPECTF--
Fatal error: Declaration of B::methodScalar1(): C|array must be compatible with A::methodScalar1(): static|bool in %s on line %d
