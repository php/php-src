--TEST--
Generator return type must be a supertype of Generator (with union types)
--FILE--
<?php
function test1() : StdClass|ArrayObject|array {
    yield 1;
}
?>
--EXPECTF--
Fatal error: test1(): Generator return type must be a supertype of Generator, StdClass|ArrayObject|array declared in %s on line %d
