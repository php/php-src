--TEST--
Abstract generic type cannot be in union (simple union with built-in type)
--FILE--
<?php

interface I<T> {
    public function foo(T|int $param): T|int;
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type in %s on line %d
