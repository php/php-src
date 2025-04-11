--TEST--
Associated type cannot be in union (simple union with class type)
--FILE--
<?php

interface I {
    type T;
    public function foo(T|stdClass $param): T|stdClass;
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of a union type in %s on line %d
