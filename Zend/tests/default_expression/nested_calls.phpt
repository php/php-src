--TEST--
Tests passing default in a parent argument position that is less than the child call's total number of arguments
--FILE--
<?php

function F($X = 1, $Y = 2) {
	return $X + $Y;
}
var_dump(F(F(0, 1) + default));
?>
--EXPECTF--
int(4)
