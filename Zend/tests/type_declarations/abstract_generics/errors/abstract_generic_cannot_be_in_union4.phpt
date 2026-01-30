--TEST--
Abstract generic type cannot be in union (nullable type union with ?)
--FILE--
<?php

interface I<T> {
    public function foo(?T $param): ?T;
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type in %s on line %d
