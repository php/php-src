--TEST--
intadd functionality
--FILE--
<?php
var_dump(intadd(41, 1)           === 42);
var_dump(intadd(-1, 1)           === 0);
var_dump(intadd(PHP_INT_MAX,  1) === PHP_INT_MIN);
var_dump(intadd(PHP_INT_MIN, -1) === PHP_INT_MAX);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
