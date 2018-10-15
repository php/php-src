--TEST--
Generator return type must be Generator, Iterator or Traversable
--FILE--
<?php
function test1() : StdClass {
    yield 1;
}
--EXPECTF--
Fatal error: Generators may only declare a return type of Generator, Iterator, Traversable, or iterable, StdClass is not permitted in %s on line %d
