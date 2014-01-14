--TEST--
Basic arrayof tests (objects, error)
--FILE--
<?php
class My {}

function test(My[] $arrays, $o) {
	return $arrays;
}

var_dump(test([new My, new stdClass], []));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of My, stdClass found, called in %s on line %d and defined in %s on line %d


