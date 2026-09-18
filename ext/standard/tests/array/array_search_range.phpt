--TEST--
array_search_range() function - basic test
--FILE--
<?php
$array = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 2, 'e' => 1];

// Search from beginning
var_dump(array_search_range(2, $array));

// Search with offset
var_dump(array_search_range(2, $array, 2));

// Search with offset and length
var_dump(array_search_range(2, $array, 0, 2));

// Search with strict comparison
var_dump(array_search_range('2', $array, 0, null, true));

// Search with negative offset
var_dump(array_search_range(1, $array, -2));

// Search with length
var_dump(array_search_range(3, $array, 0, 3));

// Not found
var_dump(array_search_range(10, $array));
?>
--EXPECT--
string(1) "b"
string(1) "d"
string(1) "b"
bool(false)
string(1) "e"
string(1) "c"
bool(false)
