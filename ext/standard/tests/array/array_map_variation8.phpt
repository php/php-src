--TEST--
Test array_map() function : usage variations - array with references
--FILE--
<?php
/*
 * Test array_map() by passing array having reference values for $arr1 argument
 */

echo "*** Testing array_map() : array with references for 'arr1' argument ***\n";

function callback1($a)
{
  return ($a);
}

function callback_cat($a, $b)
{
  return ($a . $b);
}

// reference variables
$value1 = 10;
$value2 = "hello";
$value3 = 0;
$value4 = &$value2;

// array containing reference variables
$arr1 = array(
  0 => 0,
  1 => &$value4,
  2 => &$value2,
  3 => "hello",
  4 => &$value3,
  $value4 => &$value2
);
echo "-- with one array --\n";
var_dump( array_map('callback1', $arr1) );

echo "-- with two arrays --\n";
var_dump( array_map('callback_cat', $arr1, $arr1) );

echo "Done";
?>
--EXPECT--
*** Testing array_map() : array with references for 'arr1' argument ***
-- with one array --
array(6) {
  [0]=>
  int(0)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "hello"
  [3]=>
  string(5) "hello"
  [4]=>
  int(0)
  ["hello"]=>
  string(5) "hello"
}
-- with two arrays --
array(6) {
  [0]=>
  string(2) "00"
  [1]=>
  string(10) "hellohello"
  [2]=>
  string(10) "hellohello"
  [3]=>
  string(10) "hellohello"
  [4]=>
  string(2) "00"
  [5]=>
  string(10) "hellohello"
}
Done
