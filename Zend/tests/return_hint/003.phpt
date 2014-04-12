--TEST--
Basic return hints at compilation (constant scalar)
--FILE--
<?php
function test1() : array {
	return 1;
}
?>
--EXPECTF--
Fatal error: the function test1 was expected to return an array and returned an integer in %s on line %d
