--TEST--
intersection disallow generator
--FILE--
<?php
function () : Generator & stdClass {
	yield $thing;
};
?>
--EXPECTF--
Fatal error: Generators may only declare a return type of Generator, Iterator or Traversable, intersections are not permitted in %s on line 3
