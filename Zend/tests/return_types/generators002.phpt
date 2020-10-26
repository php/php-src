--TEST--
Generator return type must be a supertype of Generator
--FILE--
<?php
function test1() : StdClass {
    yield 1;
}
?>
--EXPECTF--
Fatal error: test1(): Generator return type must be a supertype of Generator, StdClass declared in %s on line %d
