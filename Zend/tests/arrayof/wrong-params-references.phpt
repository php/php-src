--TEST--
test arrayof reference
--FILE--
<?php
function test(array[] &$arrays) {
	return $arrays;
}

var_dump(test([array(), 1]));
?>
--EXPECTF--
Fatal error: Only variables can be passed by reference in %s on line %d



