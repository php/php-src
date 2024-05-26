--TEST--
Test for any_empty() function
--FILE--
<?php
$array1 = array(0, 1, 2);
$array2 = array(1, 2, 3);
$array3 = array('', 'test', 'foo');
$array4 = array('test', 'foo', 'bar');

var_dump(any_empty($array1)); // Expected: bool(true)
var_dump(any_empty($array2)); // Expected: bool(false)
var_dump(any_empty($array3)); // Expected: bool(true)
var_dump(any_empty($array4)); // Expected: bool(false)
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
