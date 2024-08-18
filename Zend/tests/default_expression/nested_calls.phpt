--TEST--
Test passing default in a parent argument position that is less than the child call's total number of arguments
--FILE--
<?php

function F($x = 1, $y = 2) {
	return $x + $y;
}
var_dump(F(F(0, 1) + default));
?>
--EXPECTF--
int(4)
