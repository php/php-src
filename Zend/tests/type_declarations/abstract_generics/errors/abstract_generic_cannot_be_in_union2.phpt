--TEST--
Abstract generic type cannot be in union (simple union with class type)
--FILE--
<?php

interface I<T> {
    public function foo(T|stdClass $param): T|stdClass;
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type in %s on line %d
