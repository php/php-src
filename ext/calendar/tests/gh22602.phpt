--TEST--
Bug GH-22602: (gregoriantojd() integer overflow with INT_MAX year)
--EXTENSIONS--
calendar
--FILE--
<?php
$max = PHP_INT_MAX > 2147483647 ? 2147483647 : PHP_INT_MAX;
$min = PHP_INT_MAX > 2147483647 ? -2147483648 : PHP_INT_MIN;

var_dump(gregoriantojd(5, 5, $max));
var_dump(gregoriantojd(5, 5, $min));
var_dump(juliantojd(5, 5, 2147483647));
var_dump(juliantojd(5, 5, -2147483647));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
