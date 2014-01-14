--TEST--
Basic arrayof test (hint for array, short syntax, error)
--FILE--
<?php
function test([] $array) {
	return $array;
}

var_dump(test(1));
?>
--EXPECTF--
Fatal error: Array of type hints must specify a type in %s on line %d


