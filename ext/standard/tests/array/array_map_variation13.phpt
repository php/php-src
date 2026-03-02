--TEST--
Test array_map() function : usage variations - callback function with different return types
--FILE--
<?php
/*
 * Test array_map() by passing different callback function returning:
 *   int, string, bool, null values
 */

echo "*** Testing array_map() : callback with diff return value ***\n";

$array1 = array(1, 2, 3);
$array2 = array(3, 4, 5);

echo "-- with integer return value --\n";
function callback_int($a, $b)
{
  return $a + $b;
}
var_dump( array_map('callback_int', $array1, $array2));

echo "-- with string return value --\n";
function callback_string($a, $b)
{
  return "$a"."$b";
}
var_dump( array_map('callback_string', $array1, $array2));

echo "-- with bool return value --\n";
function callback_bool($a, $b)
{
  return TRUE;
}
var_dump( array_map('callback_bool', $array1, $array2));

echo "-- with null return value --\n";
function callback_null($array1)
{
  return NULL;
}
var_dump( array_map('callback_null', $array1));

echo "-- with no return value --\n";
function callback_without_ret($arr1)
{
  echo "callback_without_ret called\n";
}
var_dump( array_map('callback_without_ret', $array1));

echo "Done";
?>
--EXPECT--
*** Testing array_map() : callback with diff return value ***
-- with integer return value --
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(6)
  [2]=>
  int(8)
}
-- with string return value --
array(3) {
  [0]=>
  string(2) "13"
  [1]=>
  string(2) "24"
  [2]=>
  string(2) "35"
}
-- with bool return value --
array(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(true)
}
-- with null return value --
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
-- with no return value --
callback_without_ret called
callback_without_ret called
callback_without_ret called
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
Done
