--TEST--
Test array_diff() function : usage variations - comparing integers, float
and string array values
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are not
 * present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

/*
 * Test how array_diff compares integers, floats and strings
 */

echo "*** Testing array_diff() : usage variations ***\n";

$arr_int = array(1, 2, 3);
$arr_float = array(1.00, 2.00, 3.00);
$arr_int_str = array('1', '2', '3');
$arr_float_str = array('1.00', '2.00', '3.00');

print "-- Compare integers and floats: --\n";
var_dump(array_diff($arr_int, $arr_float));
var_dump(array_diff($arr_float, $arr_int));


print "-- Compare integers and strings containing an integers: --\n";
var_dump(array_diff($arr_int, $arr_int_str));
var_dump(array_diff($arr_int_str, $arr_int));

print "-- Compare integers and strings containing floats: --\n";
var_dump(array_diff($arr_int, $arr_float_str));
var_dump(array_diff($arr_float_str, $arr_int));

print "-- Compare floats and strings containing integers: --\n";

var_dump(array_diff($arr_float, $arr_int_str));
var_dump(array_diff($arr_int_str, $arr_float));

print "-- Compare floats and strings containing floats: --\n";
var_dump(array_diff($arr_float, $arr_float_str));
var_dump(array_diff($arr_float_str, $arr_float));

print "-- Compare strings containing integers and strings containing floats: --\n";
var_dump(array_diff($arr_int_str, $arr_float_str));
var_dump(array_diff($arr_float_str, $arr_int_str));

echo "Done";
?>
--EXPECT--
*** Testing array_diff() : usage variations ***
-- Compare integers and floats: --
array(0) {
}
array(0) {
}
-- Compare integers and strings containing an integers: --
array(0) {
}
array(0) {
}
-- Compare integers and strings containing floats: --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  string(4) "1.00"
  [1]=>
  string(4) "2.00"
  [2]=>
  string(4) "3.00"
}
-- Compare floats and strings containing integers: --
array(0) {
}
array(0) {
}
-- Compare floats and strings containing floats: --
array(3) {
  [0]=>
  float(1)
  [1]=>
  float(2)
  [2]=>
  float(3)
}
array(3) {
  [0]=>
  string(4) "1.00"
  [1]=>
  string(4) "2.00"
  [2]=>
  string(4) "3.00"
}
-- Compare strings containing integers and strings containing floats: --
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
array(3) {
  [0]=>
  string(4) "1.00"
  [1]=>
  string(4) "2.00"
  [2]=>
  string(4) "3.00"
}
Done
