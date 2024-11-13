--TEST--
Test array_sum() function with empty array
--FILE--
<?php
$input = [];

echo "array_sum() version:\n";
var_dump(array_sum($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry + $value, 0));
?>
--EXPECT--
array_sum() version:
int(0)
array_reduce() version:
int(0)
