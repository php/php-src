--TEST--
Test ksort() function : usage variations - sort array with diff. sub arrays
--FILE--
<?php
/*
 * testing ksort() by providing arrays containing sub arrays for $array argument
 * with flowing flag values:
 *  1. flag value as default
 *  2. SORT_REGULAR - compare items normally
*/

echo "*** Testing ksort() : usage variations ***\n";

// array with diff sub arrays to be sorted
$various_arrays = array (
  // null array
  1  => array(),

  // array contains null sub array
  2 => array( 1 => array() ),

  // array of arrays along with some values
  3 => array(4 => 44, 1 => 11, 3 => array(64,61) ),

  // array contains sub arrays
  4 => array ( 3 => array(33,-5,6), 1 => array(11),
               2 => array(22,-55), 0  => array() )
);


$count = 1;
echo "\n-- Testing ksort() by supplying various arrays containing sub arrays --\n";

// loop through to test ksort() with different arrays
foreach ($various_arrays as $array) {

  echo "\n-- Iteration $count --\n";
  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump( ksort($temp_array) );
  var_dump($temp_array);

  echo "- Sort flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump( ksort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying various arrays containing sub arrays --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(0) {
}
- Sort flag = SORT_REGULAR -
bool(true)
array(0) {
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(1) {
  [1]=>
  array(0) {
  }
}
- Sort flag = SORT_REGULAR -
bool(true)
array(1) {
  [1]=>
  array(0) {
  }
}

-- Iteration 3 --
- With default sort flag -
bool(true)
array(3) {
  [1]=>
  int(11)
  [3]=>
  array(2) {
    [0]=>
    int(64)
    [1]=>
    int(61)
  }
  [4]=>
  int(44)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(3) {
  [1]=>
  int(11)
  [3]=>
  array(2) {
    [0]=>
    int(64)
    [1]=>
    int(61)
  }
  [4]=>
  int(44)
}

-- Iteration 4 --
- With default sort flag -
bool(true)
array(4) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(1) {
    [0]=>
    int(11)
  }
  [2]=>
  array(2) {
    [0]=>
    int(22)
    [1]=>
    int(-55)
  }
  [3]=>
  array(3) {
    [0]=>
    int(33)
    [1]=>
    int(-5)
    [2]=>
    int(6)
  }
}
- Sort flag = SORT_REGULAR -
bool(true)
array(4) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(1) {
    [0]=>
    int(11)
  }
  [2]=>
  array(2) {
    [0]=>
    int(22)
    [1]=>
    int(-55)
  }
  [3]=>
  array(3) {
    [0]=>
    int(33)
    [1]=>
    int(-5)
    [2]=>
    int(6)
  }
}
Done
