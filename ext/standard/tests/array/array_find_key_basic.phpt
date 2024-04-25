--TEST--
basic array_find_key test
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

var_dump(array_find_key($array1, fn($value) => $value > 3));
var_dump(array_find_key($array2, fn($value) => $value > 3));
var_dump(array_find_key($array2, fn($value) => $value > 5));
var_dump(array_find_key([], fn($value) => true));
var_dump(array_find_key($array1, fn($value, $key) => $key === "c"));
var_dump(array_find_key($array1, fn($value, $key) => false));
?>
--EXPECT--
string(1) "d"
int(3)
NULL
NULL
string(1) "c"
NULL
