--TEST--
Test uasort() function : usage variations - sort array having subarrays
--FILE--
<?php
/*
* Testing uasort() with 'array_arg' having different subarrays as array elements
*/

// comparison function
function cmp_function($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

echo "*** Testing uasort() : sorting array having different subarrays ***\n";

$array_args = array(
  0 => array(2, 10, -1),
  1 => array(100),
  2 => array(),
  3 => array(0),
  4 => array(-1),
  5 => array(-9, 34, 54, 0, 20),
  6 => array(''),
  7 => array("apple", "Apple", "APPLE", "aPPle", "aPpLe")
);
$temp_array = $array_args;
// sorting array_arg as whole array
var_dump( uasort($temp_array, 'cmp_function') );  // expecting: bool(true)
var_dump($temp_array);

?>
--EXPECT--
*** Testing uasort() : sorting array having different subarrays ***
bool(true)
array(8) {
  [2]=>
  array(0) {
  }
  [6]=>
  array(1) {
    [0]=>
    string(0) ""
  }
  [4]=>
  array(1) {
    [0]=>
    int(-1)
  }
  [3]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(100)
  }
  [0]=>
  array(3) {
    [0]=>
    int(2)
    [1]=>
    int(10)
    [2]=>
    int(-1)
  }
  [5]=>
  array(5) {
    [0]=>
    int(-9)
    [1]=>
    int(34)
    [2]=>
    int(54)
    [3]=>
    int(0)
    [4]=>
    int(20)
  }
  [7]=>
  array(5) {
    [0]=>
    string(5) "apple"
    [1]=>
    string(5) "Apple"
    [2]=>
    string(5) "APPLE"
    [3]=>
    string(5) "aPPle"
    [4]=>
    string(5) "aPpLe"
  }
}
