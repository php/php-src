--TEST--
Test array_chunk() function : usage variations - different arrays  
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
 *            : Chunks an array into size large chunks
 * Source code: ext/standard/array.c
*/

/*
 * Testing array_chunk() function with following conditions 
 *   1. array without elements
 *   2. associative array with duplicate keys
 *   3. array with one element 
*/

echo "*** Testing array_chunk() : usage variations ***\n";

// input array
$input_arrays = array (

  // array without elements
  "array1" => array(),

  // array with one element
  "array2" => array(1),
  
  // associative array with duplicate keys
  "array3" => array("a" => 1, "b" => 2, "c" => 3, "a" => 4, "d" => 5)

);

$size = 2;
$count = 1;

echo "\n-- Testing array_chunk() by supplying various arrays --\n";

// loop through the array for 'array' argument
foreach ($input_arrays as $input_array){
  echo "\n-- Iteration $count --\n";
  var_dump( array_chunk($input_array, $size) );
  var_dump( array_chunk($input_array, $size, true) );
  var_dump( array_chunk($input_array, $size, false) );
  $count++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_chunk() : usage variations ***

-- Testing array_chunk() by supplying various arrays --

-- Iteration 1 --
array(0) {
}
array(0) {
}
array(0) {
}

-- Iteration 2 --
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

-- Iteration 3 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(5)
  }
}
array(2) {
  [0]=>
  array(2) {
    ["a"]=>
    int(4)
    ["b"]=>
    int(2)
  }
  [1]=>
  array(2) {
    ["c"]=>
    int(3)
    ["d"]=>
    int(5)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(5)
  }
}
Done
