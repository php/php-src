--TEST--
Negative parse_iv2 edge cases
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--FILE--
<?php

var_dump(unserialize('i:-9223372036854775808;'));
var_dump(unserialize('i:-0;'));

?>
--EXPECT--
int(-9223372036854775808)
int(0)
