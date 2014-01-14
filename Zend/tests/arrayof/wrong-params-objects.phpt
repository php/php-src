--TEST--
Basic arrayof error (objects)
--FILE--
<?php
class My {}

function test(My[] $arrays) {
	return $arrays;
}

var_dump(test([new My, new stdClass]));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of My, member of class stdClass given, called in %s on line %d and defined in %s on line %d


