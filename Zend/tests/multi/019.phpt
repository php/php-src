--TEST--
union disallow generator
--FILE--
<?php
function () : Generator | stdClass {
	yield $thing;
};
?>
--EXPECTF--
Fatal error: Generators may only declare a return type of Generator, Iterator or Traversable, unions are not permitted in %s on line 3
