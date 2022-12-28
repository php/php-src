--TEST--
Test array_product() function with empty array
--FILE--
<?php
$input = [];

echo "array_product() version:\n";
var_dump(array_product($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry * $value, 1));
?>
--EXPECTF--
array_product() version:

Deprecated: array_product(): Passing an empty array is deprecated in %s on line %d
int(1)
array_reduce() version:
int(1)
