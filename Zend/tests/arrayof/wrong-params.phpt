--TEST--
Basic arrayof test (simple types, error)
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test([array(), 1]));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays, integer found, called in %s on line %d and defined in %s on line %d


