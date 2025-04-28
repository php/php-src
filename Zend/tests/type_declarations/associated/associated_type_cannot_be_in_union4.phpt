--TEST--
Associated type cannot be in union (nullable type union with ?)
--FILE--
<?php

interface I {
    type T;
    public function foo(?T $param): ?T;
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of a union type in %s on line %d
