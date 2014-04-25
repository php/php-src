--TEST--
Generator Hinting Compile Checking Failure
--FILE--
<?php
function test1() : StdClass {
    yield 1;
}
--EXPECTF--
Fatal error: Generators may only be hinted as Generator, Iterator or Traversable, StdClass is not a valid type in %s on line %d
