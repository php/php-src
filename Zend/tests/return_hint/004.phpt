--TEST--
Basic return hints at execution mismatched basic type
--FILE--
<?php
function test1() : array {
	return "hello";
}

test1()
?>
--EXPECTF--
Fatal error: the function test1 was expected to return array and returned string in %s on line %d
