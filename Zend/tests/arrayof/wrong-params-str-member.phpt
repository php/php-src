--TEST--
test null passed in arrayof arg (string key)
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test(["first" => null]));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays, null at element "first", called in %s on line %d and defined in %s on line %d



