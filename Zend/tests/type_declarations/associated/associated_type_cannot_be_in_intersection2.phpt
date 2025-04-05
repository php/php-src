--TEST--
Associated type cannot be in intersection (DNF type)
--FILE--
<?php

interface I {
    type T;
    public function foo(stdClass|(T&Traversable) $param): stdClass|(T&Traversable);
}

?>
--EXPECTF--
Fatal error: Associated type cannot be part of an intersection type in %s on line %d
