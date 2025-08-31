--TEST--
Generator return type must be a supertype of Generator
--FILE--
<?php
function test1() : StdClass {
    yield 1;
}
?>
--EXPECTF--
Fatal error: Generator return type must be a supertype of Generator, StdClass given in %s on line %d
