--TEST--
Test krsort() function : usage variations - sort array with diff. sub arrays
--FILE--
<?php
/* Prototype  : bool krsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
 * testing krsort() by providing arrays contains sub arrays for $array argument
 * with flowing flag values
 *  1.flag  value as defualt
 *  2.SORT_REGULAR - compare items normally
*/

echo "*** Testing krsort() : usage variations ***\n";

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
echo "\n-- Testing krsort() by supplying various arrays containing sub arrays --\n";

// loop through to test krsort() with different arrays
foreach ($various_arrays as $array) {

  echo "\n-- Iteration $count --\n";
  echo "- With defualt sort flag -\n";
  $temp_array = $array;
  var_dump( krsort($temp_array) );
  var_dump($temp_array);

  echo "- Sort flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump( krsort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying various arrays containing sub arrays --

-- Iteration 1 --
- With defualt sort flag -
bool(true)
array(0) {
}
- Sort flag = SORT_REGULAR -
bool(true)
array(0) {
}

-- Iteration 2 --
- With defualt sort flag -
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
- With defualt sort flag -
bool(true)
array(3) {
  [4]=>
  int(44)
  [3]=>
  array(2) {
    [0]=>
    int(64)
    [1]=>
    int(61)
  }
  [1]=>
  int(11)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(3) {
  [4]=>
  int(44)
  [3]=>
  array(2) {
    [0]=>
    int(64)
    [1]=>
    int(61)
  }
  [1]=>
  int(11)
}

-- Iteration 4 --
- With defualt sort flag -
bool(true)
array(4) {
  [3]=>
  array(3) {
    [0]=>
    int(33)
    [1]=>
    int(-5)
    [2]=>
    int(6)
  }
  [2]=>
  array(2) {
    [0]=>
    int(22)
    [1]=>
    int(-55)
  }
  [1]=>
  array(1) {
    [0]=>
    int(11)
  }
  [0]=>
  array(0) {
  }
}
- Sort flag = SORT_REGULAR -
bool(true)
array(4) {
  [3]=>
  array(3) {
    [0]=>
    int(33)
    [1]=>
    int(-5)
    [2]=>
    int(6)
  }
  [2]=>
  array(2) {
    [0]=>
    int(22)
    [1]=>
    int(-55)
  }
  [1]=>
  array(1) {
    [0]=>
    int(11)
  }
  [0]=>
  array(0) {
  }
}
Done
