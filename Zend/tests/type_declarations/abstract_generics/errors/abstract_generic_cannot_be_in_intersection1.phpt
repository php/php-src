--TEST--
Abstract generic type cannot be in intersection (simple intersection with class type)
--FILE--
<?php

interface I<T> {
    public function foo(T&Traversable $param): T&Traversable;
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of an intersection type in %s on line %d
