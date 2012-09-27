--TEST--
Yielding the result of a non-ref function call throw a notice
--FILE--
<?php

function foo() {
    return "bar";
}

function &gen() {
    yield foo();
}

$gen = gen();
var_dump($gen->current());

?>
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d
string(3) "bar"
