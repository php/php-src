--TEST--
Test list_filter() function
--FILE--
<?php

$array1 = [1,0,2,3,4];
$array2 = ['a' => 1, 'b' => 2, 'c' => 3];

var_dump(array_is_list($array1));
var_dump(array_is_list(array_filter($array1)));
var_dump(array_is_list(list_filter($array1)));


var_dump(array_is_list($array2));
var_dump(array_is_list(array_filter($array2)));
var_dump(array_is_list(list_filter($array2)));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
