--TEST--
Test array_product() function: resources in array
--FILE--
<?php
$input = [10, STDERR /* Should get casted to 3 as an integer */];

echo "array_product() version:\n";
var_dump(array_product($input));

echo "array_reduce() version:\n";
try {
    var_dump(array_reduce($input, fn($carry, $value) => $carry * $value, 1));
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
array_product() version:

Warning: array_product(): Multiplication is not supported on type resource in %s on line %d
int(30)
array_reduce() version:
Unsupported operand types: int * resource
