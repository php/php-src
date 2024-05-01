--TEST--
basic array_any test
--FILE--
<?php
$array1 = [
    "a" => 1,
    "b" => 2,
    "c" => 3,
    "d" => 4,
    "e" => 5,
];
$array2 = [
    1, 2, 3, 4, 5
];

var_dump(array_all($array1, fn($value) => $value > 0));
var_dump(array_all($array2, fn($value) => $value > 0));
var_dump(array_all($array2, fn($value) => $value > 1));
var_dump(array_all([], fn($value) => true));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
