--TEST--
array_str_contains() basic functionality and edge cases
--FILE--
<?php

$fruits = ["apple", "banana", "cherry"];

// 1. Basic match
var_dump(array_str_contains($fruits, "apple"));
var_dump(array_str_contains($fruits, "banana"));
var_dump(array_str_contains($fruits, "cherry"));

// 2. No match
var_dump(array_str_contains($fruits, "orange"));
var_dump(array_str_contains($fruits, "app"));

// 3. Empty needle & empty array
var_dump(array_str_contains($fruits, ""));
var_dump(array_str_contains([], "apple"));
var_dump(array_str_contains([], ""));

// 4. Case-sensitivity
var_dump(array_str_contains($fruits, "APPLE"));
var_dump(array_str_contains($fruits, "Banana"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
