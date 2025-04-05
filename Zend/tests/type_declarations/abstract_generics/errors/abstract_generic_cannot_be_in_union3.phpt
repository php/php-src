--TEST--
Abstract generic type cannot be in union (DNF type)
--FILE--
<?php

interface I<T> {
    public function foo(T|(Traversable&Countable) $param): T|(Traversable&Countable);
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type in %s on line %d
