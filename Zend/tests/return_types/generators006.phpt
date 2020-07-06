--TEST--
Generator return type must be a supertype of Generator (with union types)
--FILE--
<?php
function test1() : StdClass|ArrayObject|array {
    yield 1;
}
--EXPECTF--
Fatal error: Generator return type must be a supertype of Generator, StdClass|ArrayObject|array returned in %s on line %d
