--TEST--
Associated type cannot be in union (forced allowed null)
--FILE--
<?php

interface I {
    type T;
    public function foo(T $param = null): T;
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of a union type (implicitly nullable due to default null value) in %s on line %d
