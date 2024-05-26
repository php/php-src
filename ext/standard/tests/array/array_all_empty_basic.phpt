--TEST--
Test for all_empty() function
--FILE--
<?php
$array1 = array(0, '', null);
$array2 = array(1, 'test', 'foo');
$array3 = array('', '', '');

var_dump(all_empty($array1)); // Expected: bool(true)
var_dump(all_empty($array2)); // Expected: bool(false)
var_dump(all_empty($array3)); // Expected: bool(true)
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
