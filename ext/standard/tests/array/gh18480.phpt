--TEST--
GH-18480 (array_splice overflow with large offset / length values)
--FILE--
<?php
$a = [PHP_INT_MAX];
$offset = PHP_INT_MAX;
var_dump(array_splice($a,$offset, PHP_INT_MAX));
$offset = PHP_INT_MIN;
var_dump(array_splice($a,$offset, PHP_INT_MIN));
--EXPECT--
array(0) {
}
array(0) {
}

