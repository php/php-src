--TEST--
Abstract generic type cannot be in intersection (DNF type)
--FILE--
<?php

interface I<T> {
    public function foo(stdClass|(T&Traversable) $param): stdClass|(T&Traversable);
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of an intersection type in %s on line %d
