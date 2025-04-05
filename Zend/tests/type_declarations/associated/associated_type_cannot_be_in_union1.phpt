--TEST--
Associated type cannot be in union (simple union with built-in type)
--FILE--
<?php

interface I {
    type T;
    public function foo(T|int $param): T|int;
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of a union type in %s on line %d
