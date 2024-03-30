--TEST--
basic array_find test
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

var_dump(array_find($array1, fn($value) => $value > 3));
var_dump(array_find($array2, fn($value) => $value > 3));
var_dump(array_find($array2, fn($value) => $value > 5));
var_dump(array_find([], fn($value) => true));
var_dump(array_find($array1, fn($value, $key) => $key === "c"));
var_dump(array_find($array1, fn($value, $key) => false));
?>
--EXPECT--
int(4)
int(4)
NULL
NULL
int(3)
NULL
