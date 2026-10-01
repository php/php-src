--TEST--
array_sum()/array_product(): PHP 8.3 nested-array warning and array_column() integration
--FILE--
<?php

echo "-- array_column() + array_sum()/array_product() integration --\n";
$products = [
    ['name' => 'Pen',   'price' => 3],
    ['name' => 'Paper', 'price' => 5],
];
$prices = array_column($products, 'price');
var_dump($prices === [3, 5]);
var_dump(array_sum($prices) === 8);
var_dump(array_product($prices) === 15);

echo "-- PHP 8.3: nested array emits warning and is skipped --\n";
var_dump(array_sum([1, [2], 3]));
var_dump(array_product([2, [3], 4]));

?>
--EXPECTF--
-- array_column() + array_sum()/array_product() integration --
bool(true)
bool(true)
bool(true)
-- PHP 8.3: nested array emits warning and is skipped --

Warning: array_sum(): Addition is not supported on type array in %s on line %d
int(4)

Warning: array_product(): Multiplication is not supported on type array in %s on line %d
int(8)
