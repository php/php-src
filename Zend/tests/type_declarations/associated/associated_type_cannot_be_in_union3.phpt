--TEST--
Associated type cannot be in union (DNF type)
--FILE--
<?php

interface I {
    type T;
    public function foo(T|(Traversable&Countable) $param): T|(Traversable&Countable);
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of a union type in %s on line %d
