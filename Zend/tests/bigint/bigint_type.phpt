--TEST--
Type of bigint
--DESCRIPTION--
This test makes sure that bigints are reported as integers to userland, except for with debug functions
--FILE--
<?php
/* On currently supported platforms, and even on a hypothetical 128-bit platform, this will be a bigint */
$big = 2 ** 128;

var_dump($big);
var_dump(gettype($big));
var_dump(is_int($big));
var_dump(is_numeric($big));

echo PHP_EOL;

debug_zval_dump($big);
?>
--EXPECTF--
int(%s)
string(7) "integer"
bool(true)
bool(true)

bigint(%s) refcount(%d)