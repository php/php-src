--TEST--
Associated type cannot be in intersection (simple intersection with class type)
--FILE--
<?php

interface I {
    type T;
    public function foo(T&Traversable $param): T&Traversable;
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of an intersection type in %s on line %d
