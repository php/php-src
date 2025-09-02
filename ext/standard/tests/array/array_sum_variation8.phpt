--TEST--
Test array_sum() function: resources in array
--FILE--
<?php
$input = [10, STDERR /* Should get casted to 3 as an integer */];

echo "array_sum() version:\n";
var_dump(array_sum($input));

echo "array_reduce() version:\n";
try {
    var_dump(array_reduce($input, fn($carry, $value) => $carry + $value, 0));
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
array_sum() version:

Warning: array_sum(): Addition is not supported on type resource in %s on line %d
int(13)
array_reduce() version:
Unsupported operand types: int + resource
