--TEST--
test allow null for arrayof args
--FILE--
<?php
function test(array[] $arrays = null) {
	return $arrays;
}

var_dump(test());
?>
--EXPECT--
NULL



