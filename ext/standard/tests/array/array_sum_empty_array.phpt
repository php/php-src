--TEST--
Test array_sum() function with empty array
--FILE--
<?php
$input = [];
var_dump(array_sum($input));
?>
--EXPECTF--
Deprecated: array_sum(): Passing an empty array is deprecated in %s on line %d
int(0)
