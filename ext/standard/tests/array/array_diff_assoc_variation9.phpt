--TEST--
Test array_diff_assoc() function : usage variations - compare multidimensional arrays
--FILE--
<?php
/*
 * Test how array_diff_assoc behaves when comparing
 * multi-dimensional arrays
 */

echo "*** Testing array_diff_assoc() : usage variations ***\n";

$array1 = array('sub_array1' => array (1, 2, 3),
                'sub_array2' => array ('a', 'b', 'c'));
$array2 = array('sub_arraya' => array (1, 3, 5),
                'sub_arrayb' => array ('a', 'z', 'y'));

echo "-- Compare two 2-D arrays --\n";
var_dump(array_diff_assoc($array1, $array2));
var_dump(array_diff_assoc($array2, $array1));

echo "\n-- Compare subarrays from two 2-D arrays --\n";
var_dump(array_diff_assoc($array1['sub_array1'], $array2['sub_arraya']));
var_dump(array_diff_assoc($array2['sub_arraya'], $array1['sub_array1']));
var_dump(array_diff_assoc($array1['sub_array2'], $array2['sub_arrayb']));
var_dump(array_diff_assoc($array2['sub_arrayb'], $array1['sub_array1']));

echo "\n-- Compare a subarray from one 2-D array and one 2-D array --\n";
var_dump(array_diff_assoc($array1['sub_array1'], $array2));
var_dump(array_diff_assoc($array1, $array2['sub_arraya']));

echo "Done";
?>
--EXPECT--
*** Testing array_diff_assoc() : usage variations ***
-- Compare two 2-D arrays --
array(2) {
  ["sub_array1"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["sub_array2"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
array(2) {
  ["sub_arraya"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  ["sub_arrayb"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "z"
    [2]=>
    string(1) "y"
  }
}

-- Compare subarrays from two 2-D arrays --
array(2) {
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [1]=>
  int(3)
  [2]=>
  int(5)
}
array(2) {
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "z"
  [2]=>
  string(1) "y"
}

-- Compare a subarray from one 2-D array and one 2-D array --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  ["sub_array1"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["sub_array2"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
Done
