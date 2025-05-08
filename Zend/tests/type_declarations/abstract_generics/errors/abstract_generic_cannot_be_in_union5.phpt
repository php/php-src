--TEST--
Abstract generic type cannot be in union (forced allowed null)
--FILE--
<?php

interface I<T> {
    public function foo(T $param = null): T;
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type (implicitly nullable due to default null value) in %s on line %d
