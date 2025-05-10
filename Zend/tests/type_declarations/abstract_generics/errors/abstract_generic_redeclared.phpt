--TEST--
Abstract generic type that is redeclared
--FILE--
<?php

interface I<T, S, T> {
    public function foo(T&Traversable $param): T&Traversable;
}

?>
--EXPECTF--
Fatal error: Duplicate generic parameter T in %s on line %d
